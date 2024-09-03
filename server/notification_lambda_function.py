import re

import boto3

client = boto3.client('dynamodb', region_name='us-west-2')


def load_notification(major, minor):
    return client.get_item(
        TableName='trippin-notifications',
        Key={
            'major': {'N': str(major)},
            'minor': {'N': str(minor)}
        }
    ).get('Item')


def to_response(body):
    return {
        'status': '200',
        'statusDescription': 'OK',
        'headers': {
            'content-type': [
                {
                    'key': 'Content-Type',
                    'value': 'text/plain'
                }
            ]
        },
        'body': body
    }


def not_found_response():
    return {
        'status': '404',
        'statusDescription': 'Not Found',
        'body': 'Not Found'
    }


def lambda_handler(event, context):
    request = event['Records'][0]['cf']['request']

    print(f'method={request["method"]}, path={request["uri"]}')

    rex = '^/notifications/([0-9]+)/([0-9]+)$'

    if request['method'] == 'GET' and (m := re.match(rex, request['uri'])):
        major, minor = m.group(1), m.group(2)
        if item := load_notification(major, minor):
            return to_response(item['notification']['S'])

    return not_found_response()
