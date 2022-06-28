import json
import boto3
import time
import datetime
import base64
import urllib.parse

client = boto3.client('dynamodb', region_name='us-west-2')


def extract_body(request):
    return json.loads(base64.b64decode(request['body']['data']).decode('utf-8'))


def validate(event):
    if 'body' not in event or 'data' not in event['body'] or 'querystring' not in event:
        return False

    params = urllib.parse.parse_qs(event['querystring'])  # querystring request property is a raw string
    record = extract_body(event)
    if 'id' not in record or 'index' not in record or 'message' not in record:
        return False
    id = record['id']
    index = record['index']
    message = record['message']

    if type(id) is not str or type(index) is not int or type(message) is not str:
        return False

    if 'h' not in params:
        return False

    hash = params['h'][0]
    hashgen = str(sum(bytearray(id + str(index), 'ascii')))
    if hash != hashgen:
        return False

    return True


def add_log_record(record):
    id = record['id']
    index = record['index']
    message = record['message']
    epoch = int(time.time())
    date = datetime.datetime.fromtimestamp(epoch).strftime('%Y-%m-%d %H:%M:%S')
    item = {
        'pk': {'N': '1'},
        'sk': {'S': f'{id}/{index}'},
        'date': {'S': date},
        'message': {'S': message}
    }
    client.put_item(TableName='trippin-logs', Item=item)
    print(f'added log entry, appid={id}, index={index}, message={message}')


def lambda_handler(event, context):
    request = event['Records'][0]['cf']['request']

    print(f'method={request["method"]}, path={request["uri"]}')

    if request['method'] == 'POST' and '/logs' in request['uri']:
        if not validate(request):
            return {'status': '400', 'statusDescription': 'Bad Request'}
        add_log_record(extract_body(request))
        return {'status': '200', 'statusDescription': 'OK'}

    return {
        'status': '404',
        'statusDescription': 'Not Found',
        'body': 'Not Found'
    }
