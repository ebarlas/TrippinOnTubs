import json
import boto3
import time
import datetime
import re

client = boto3.client('dynamodb', region_name='us-west-2')


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


def query_scores(lower, upper):
    res = client.query(
        TableName='trippin-scores',
        IndexName='pk-time-index',
        ProjectionExpression='sk',
        KeyConditionExpression='#pk = :pk AND #time BETWEEN :lower AND :upper',
        ExpressionAttributeNames={'#pk': 'pk', '#time': 'time'},
        ExpressionAttributeValues={':pk': {'N': '1'}, ':lower': {'N': str(lower)}, ':upper': {'N': str(upper)}})
    return res['Items']


def today_scores():
    upper = int(time.time())
    lower = upper - 86400
    return query_scores(lower, upper)


def today_top_scores():
    return sorted(convert(today_scores()), key=lambda s: s['score'], reverse=True)


def validate(event):
    if 'body' not in event or 'queryStringParameters' not in event:
        return False
    body = event['body']
    params = event['queryStringParameters']

    score = json.loads(body)
    if 'id' not in score or 'name' not in score or 'score' not in score:
        return False
    id = score['id']
    name = score['name']
    score = score['score']

    if type(id) is not int or type(score) is not int or type(name) is not str:
        return False
    if not re.match(r'[A-Z]{5}', name):
        return False
    if score < 0 or score > 99999:
        return False
    if id < 0 or id > 2 ** 64:
        return False

    if 'h' not in params:
        return False

    hash = params['h']
    hashgen = hex(score)[2:] + hex(id)[2:]
    if hash != hashgen:
        return False

    return True


def add_score(request):
    id = request['id']
    name = request['name']
    score = request['score']
    epoch = int(time.time())
    date = datetime.datetime.fromtimestamp(epoch).strftime('%Y-%m-%d %H:%M:%S')
    sk = '0' * (5 - len(str(score))) + str(score) + '/' + name + '/' + str(id)
    item = {
        'pk': {'N': '1'},
        'sk': {'S': sk},
        'time': {'N': str(epoch)},
        'date': {'S': date}
    }
    client.put_item(TableName='trippin-scores', Item=item)
    print(f'added score, id={id}, name={name}, score={score}')


def convert(items):
    return [{'id': int(a[2]), 'name': a[1], 'score': int(a[0])} for a in (i['sk']['S'].split('/') for i in items)]


def to_response(items):
    return {
        'statusCode': 200,
        'headers': {
            'Content-Type': 'application/json'
        },
        'body': json.dumps(items)
    }


def lambda_handler(event, context):
    print(f'method={event["httpMethod"]}, path={event["path"]}')

    if event['httpMethod'] == 'GET' and '/scores/today' in event['path']:
        return to_response(today_top_scores())

    if event['httpMethod'] == 'GET' and '/scores/alltime' in event['path']:
        return to_response(convert(top_scores(25)))

    if event['httpMethod'] == 'POST' and '/scores' in event['path']:
        if not validate(event):
            return {'statusCode': 400}
        add_score(json.loads(event['body']))
        return {'statusCode': 200}

    return {
        'statusCode': 404,
        'body': 'Not Found'
    }
