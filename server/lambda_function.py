import json
import boto3
import time
import datetime

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


def add_score(request):
    id = request['id']
    name = request['name']
    score = str(request['score'])
    epoch = int(time.time())
    date = datetime.datetime.fromtimestamp(epoch).strftime('%Y-%m-%d %H:%M:%S')
    sk = '0' * (5 - len(score)) + score + '/' + name + '/' + id
    item = {
        'pk': {'N': '1'},
        'sk': {'S': sk},
        'time': {'N': str(epoch)},
        'date': {'S': date}
    }
    res = client.put_item(TableName='trippin-scores', Item=item)


def convert(items):
    return [{'id': arr[2], 'name': arr[1], 'score': int(arr[0])} for arr in (i['sk']['S'].split('/') for i in items)]


def to_response(items):
    return {
        'statusCode': 200,
        'headers': {
            'Content-Type': 'application/json'
        },
        'body': json.dumps(items)
    }


def lambda_handler(event, context):
    if event['httpMethod'] == 'GET' and '/scores/today' in event['path']:
        return to_response(today_top_scores())

    if event['httpMethod'] == 'GET' and '/scores/alltime' in event['path']:
        return to_response(convert(top_scores(25)))

    if event['httpMethod'] == 'POST' and '/scores' in event['path']:
        add_score(json.loads(event['body']))
        return {'statusCode': 200}

    return {
        'statusCode': 404,
        'body': 'Not Found'
    }
