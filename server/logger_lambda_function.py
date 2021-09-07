import json
import boto3
import time
import datetime

client = boto3.client('dynamodb', region_name='us-west-2')


def validate(event):
    if 'body' not in event or 'queryStringParameters' not in event:
        return False
    body = event['body']
    params = event['queryStringParameters']

    record = json.loads(body)
    if 'id' not in record or 'time' not in record or 'index' not in record or 'message' not in record:
        return False
    id = record['id']
    start_time = record['time']
    index = record['index']
    message = record['message']

    if type(id) is not int or type(start_time) is not int or type(index) is not int or type(message) is not str:
        return False
    if id < 0 or id > 2 ** 64:
        return False

    if 'h' not in params:
        return False

    hash = params['h']
    hashgen = hex(id)[2:] + hex(start_time)[2:] + hex(index)[2:]
    if hash != hashgen:
        return False

    return True


def add_log_record(record):
    id = record['id']
    start_time = record['time']
    index = record['index']
    message = record['message']
    epoch = int(time.time())
    date = datetime.datetime.fromtimestamp(epoch).strftime('%Y-%m-%d %H:%M:%S')
    item = {
        'appid': {'S': f'{start_time}-{id}'},
        'index': {'N': str(index)},
        'time': {'N': str(epoch)},
        'date': {'S': date},
        'message': {'S': message}
    }
    client.put_item(TableName='trippin-logs', Item=item)
    print(f'added log entry, appid={start_time}-{id}, index={index}, message={message}')


def lambda_handler(event, context):
    print(f'method={event["httpMethod"]}, path={event["path"]}')

    if event['httpMethod'] == 'POST' and '/logs' in event['path']:
        if not validate(event):
            return {'statusCode': 400}
        add_log_record(json.loads(event['body']))
        return {'statusCode': 200}

    return {
        'statusCode': 404,
        'body': 'Not Found'
    }
