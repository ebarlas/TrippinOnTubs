import datetime
import re
import time
import zoneinfo

import boto3

client = boto3.client('dynamodb', region_name='us-west-2')

table = 'trippin-logs'

date = datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')
print(f'epoch: {date}')


def query(date, last_key=None):
    params = {
        'TableName': table,
        'IndexName': 'date-index',
        'Limit': 50,
        'KeyConditionExpression': 'pk = :pk and #date > :date',
        'ExpressionAttributeNames': {
            '#date': 'date'
        },
        'ExpressionAttributeValues': {
            ':pk': {
                'N': '1'
            },
            ':date': {
                'S': date
            }
        }
    }
    if last_key:
        params['ExclusiveStartKey'] = last_key
    return client.query(**params)


def convert(date):
    pattern = r'(\d+)-(\d+)-(\d+) (\d+):(\d+):(\d+)'
    tz = zoneinfo.ZoneInfo('America/Los_Angeles')
    utc = datetime.timezone.utc
    m = re.match(pattern, date)
    yr, mon, day, hr, min, sec = [int(g) for g in m.groups()]
    d = datetime.datetime(year=yr, month=mon, day=day, hour=hr, minute=min, second=sec, tzinfo=utc).astimezone(tz=tz)
    return d.strftime('%Y-%m-%d %H:%M:%S')


def print_item(item):
    id, index = item['sk']['S'].split('/')
    date = convert(item['date']['S'])
    msg = item['message']['S']
    print(f'[{date}] id={id}, index={index}, {msg}')


def item_sort_key(item):
    d = item["date"]["S"]
    id, index = item['sk']['S'].split('/')
    return d, id, int(index)


def scroll(date):
    last_key = None
    while True:
        res = query(date, last_key)
        if not res.get('Items'):
            print('sleeping...')
            time.sleep(5)
            continue
        items = res['Items']
        items.sort(key=item_sort_key)
        for item in items:
            print_item(item)
        last_item = items[-1]
        last_key = {'pk': {'N': '1'}, 'sk': last_item['sk'], 'date': last_item['date']}


scroll(date)
