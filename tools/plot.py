import collections
import datetime
import re
import sys
import zoneinfo

import boto3
import matplotlib.pyplot as plt

client = boto3.client('dynamodb', region_name='us-west-2')

table = 'trippin-logs'

pac = zoneinfo.ZoneInfo('America/Los_Angeles')
utc = datetime.timezone.utc

date_fmt_sec_res = '%Y-%m-%d %H:%M:%S'
date_fmt_compact = '%Y%m%dT%H%M%S'

opts = {
    'second': {
        'fmt': '%Y-%m-%d %H:%M:%S',
        'period': datetime.timedelta(seconds=1)
    },
    'minute': {
        'fmt': '%Y-%m-%d %H:%M',
        'period': datetime.timedelta(minutes=1)
    },
    'hour': {
        'fmt': '%Y-%m-%d %H',
        'period': datetime.timedelta(hours=1)
    },
    'day': {
        'fmt': '%Y-%m-%d',
        'period': datetime.timedelta(days=1)
    },
    'month': {
        'fmt': '%Y-%m'
    },
    'year': {
        'fmt': '%Y'
    }
}

filters = {
    'all': lambda item: True,
    'launch': lambda item: 'op=init' in item['message']['S'],
    'game': lambda item: 'op=state_change, prev=START_MENU, next=PLAYING' in item['message']['S'],
    'name': lambda item: 'prev=NAME_FORM, next=START_MENU, name=' in item['message']['S']
}


def query(start_date, end_date, last_key=None):
    params = {
        'TableName': table,
        'IndexName': 'date-index',
        'Limit': 50,
        'KeyConditionExpression': 'pk = :pk and #date between :start_date and :end_date',
        'ExpressionAttributeNames': {
            '#date': 'date'
        },
        'ExpressionAttributeValues': {
            ':pk': {
                'N': '1'
            },
            ':start_date': {
                'S': start_date
            },
            ':end_date': {
                'S': end_date
            }
        }
    }
    if last_key:
        params['ExclusiveStartKey'] = last_key
    return client.query(**params)


def query_all(start_date, end_date):
    all_items = []
    last_key = None
    n = 1
    while True:
        res = query(start_date, end_date, last_key)
        if not res.get('Items'):
            return all_items
        items = res['Items']
        all_items.extend(items)
        first_date = items[0]["date"]["S"]
        last_item = items[-1]
        last_date = last_item["date"]["S"]
        print(f'n={n}, page={len(items)}, total={len(all_items)}, first={first_date}, last={last_date}')
        last_key = {'pk': {'N': '1'}, 'sk': last_item['sk'], 'date': last_item['date']}
        n += 1


def make_db_date(date_str):
    return datetime.datetime.strptime(date_str, date_fmt_sec_res).astimezone(tz=utc).strftime(date_fmt_sec_res)


def convert_from_db_fmt(date_str, fmt):
    pattern = r'(\d+)-(\d+)-(\d+) (\d+):(\d+):(\d+)'
    tz = zoneinfo.ZoneInfo('America/Los_Angeles')
    utc = datetime.timezone.utc
    m = re.match(pattern, date_str)
    yr, mon, day, hr, min, sec = [int(g) for g in m.groups()]
    d = datetime.datetime(year=yr, month=mon, day=day, hour=hr, minute=min, second=sec, tzinfo=utc).astimezone(tz=tz)
    return d.strftime(fmt)


def switch_format(date_str, fmt):
    return datetime.datetime.strptime(date_str, date_fmt_sec_res).strftime(fmt)


def date_range(start_date, end_date, delta, target_fmt):
    sdt = datetime.datetime.strptime(start_date, date_fmt_sec_res)
    edt = datetime.datetime.strptime(end_date, date_fmt_sec_res)
    arr = []
    while sdt < edt:
        arr.append(sdt.strftime(target_fmt))
        sdt = sdt + delta
    return arr


def plot(x, y, title, file):
    plt.figure(figsize=(16, 10))
    plt.bar(x, y, color='blue')
    plt.xlabel('Time')
    plt.ylabel('Events')
    plt.title(title)
    plt.xticks(rotation=90)
    plt.subplots_adjust(bottom=0.2)
    plt.grid()
    plt.savefig(file)


def cap_first_letter(s):
    return f'{s[0:1].upper()}{s[1:]}'


def main():
    if len(sys.argv) != 5:
        print('args: [start] [end] [bucket] [filter]')
        sys.exit(1)
    start, end, bucket, filter_name  = sys.argv[1:]
    filter_fn = filters[filter_name]
    all_items = query_all(make_db_date(start), make_db_date(end))
    items = [item for item in all_items if filter_fn(item)]
    opt = opts[bucket]
    target_fmt = opt['fmt']
    target_period = opt.get('period')
    compact_fmt = target_fmt.replace('-', '').replace(':', '').replace(' ', 'T')
    item_dates = [convert_from_db_fmt(item['date']['S'], target_fmt) for item in items]
    counter = collections.Counter(item_dates)
    if target_period:
        x = date_range(start, end, target_period, target_fmt)
    else:
        x = sorted(list(counter.keys()))
    y = [counter.get(i, 0) for i in x]
    start_title = switch_format(start, target_fmt)
    end_title = switch_format(end, target_fmt)
    bucket_title = cap_first_letter(bucket)
    filter_title = cap_first_letter(filter_name)
    plot_title = f'{filter_title} Events - {start_title} to {end_title} by {bucket_title} - {len(items)} Total'
    start_file = switch_format(start, compact_fmt)
    end_file = switch_format(end, compact_fmt)
    plot_file = f'tot_chart_{start_file}_{end_file}_{bucket}_{filter_name}.png'
    plot(x, y, plot_title, plot_file)
    print(plot_file)


main()
