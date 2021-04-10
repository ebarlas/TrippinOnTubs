import datetime
import time
import random
import boto3

epoch = int(time.time())
print(epoch)

now = datetime.datetime.fromtimestamp(epoch)
date = now.strftime('%Y-%m-%d %H:%M:%S')
print(date)

client = boto3.client('dynamodb', region_name='us-west-2')

alphabet = ''.join(chr(n) for n in range(65, 91))


def rand_name():
    return ''.join(random.choice(alphabet) for _ in range(5))


def convert(items):
    return [{'name': arr[1], 'score': int(arr[0])} for arr in (i['sk']['S'].split('/') for i in items)]


# for n in range(10):
#     name = rand_name()
#     score = str(random.randint(1, 99999))
#     sk = '0' * (5 - len(score)) + score + '/' + name + '/' + str(epoch)
#     item = {
#         'pk': {'N': '1'},
#         'sk': {'S': sk},
#         'time': {'N': str(epoch)},
#         'date': {'S': date}
#     }
#     res = client.put_item(TableName='trippin-scores', Item=item)


def top_scores(limit):
    res = client.query(
        TableName='trippin-scores',
        ScanIndexForward=False,
        Limit=limit,
        ProjectionExpression='sk',
        KeyConditionExpression='#pk = :pk',
        ExpressionAttributeNames={'#pk': 'pk'},
        ExpressionAttributeValues={':pk': {'N': '1'}})
    return res['Items']


items = top_scores(5)
for i in items:
    print(i)


def query_scores(lower, upper, start_key):
    kwargs = {
        'TableName': 'trippin-scores',
        'IndexName': 'pk-time-index',
        'ProjectionExpression': 'sk',
        'KeyConditionExpression': '#pk = :pk AND #time BETWEEN :lower AND :upper',
        'ExpressionAttributeNames': {'#pk': 'pk', '#time': 'time'},
        'ExpressionAttributeValues': {':pk': {'N': '1'}, ':lower': {'N': str(lower)}, ':upper': {'N': str(upper)}}
    }
    if start_key:
        kwargs['ExclusiveStartKey'] = start_key
    return client.query(**kwargs)


def today_scores():
    upper = int(time.time())
    lower = upper - 86400
    items = []
    start_key = None
    while True:
        res = query_scores(lower, upper, start_key)
        items.extend(res['Items'])
        if 'LastEvaluatedKey' in res:
            start_key = res['LastEvaluatedKey']
        else:
            break
    return items


def today_top_scores(limit):
    return sorted(convert(today_scores()), key=lambda s: s['score'], reverse=True)[0:limit]


items = today_scores()
for i in items:
    print(i)

top = today_top_scores(3)
for s in top:
    print(s)
