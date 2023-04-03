import json
import boto3
import time
import datetime
import re
import base64
import urllib.parse

client = boto3.client('dynamodb', region_name='us-west-2')


def top_scores(version, limit):
    res = client.query(
        TableName='trippin-scores',
        IndexName='score-index',
        ScanIndexForward=False,
        Limit=limit,
        KeyConditionExpression='#pk = :pk',
        ExpressionAttributeNames={'#pk': 'pk'},
        ExpressionAttributeValues={':pk': {'N': str(version)}})
    return res['Items']


def top_today_scores(version, day, limit):
    res = client.query(
        TableName='trippin-scores',
        IndexName='dayscore-index',
        ScanIndexForward=False,
        Limit=limit,
        KeyConditionExpression='#pk = :pk AND begins_with (#dayscore, :dayscore)',
        ExpressionAttributeNames={'#pk': 'pk', '#dayscore': 'dayscore'},
        ExpressionAttributeValues={':pk': {'N': str(version)}, ':dayscore': {'S': day}})
    return res['Items']


def extract_add_body(request):
    return json.loads(base64.b64decode(request['body']['data']).decode('utf-8'))


def validate_input_event(event):
    if type(event) is not dict:
        return False
    if 't' not in event or 'e' not in event:
        return False
    if type(event['t']) is not int or type(event['e']) is not int:
        return False
    return True


def validate_input_events(events):
    if type(events) is not list:
        return False
    for e in events:
        if not validate_input_event(e):
            return False
    return True


def validate_add(event):
    if 'body' not in event or 'data' not in event['body'] or 'querystring' not in event:
        return False

    params = urllib.parse.parse_qs(event['querystring'])  # querystring request property is a raw string
    body = extract_add_body(event)
    if 'version' not in body or 'id' not in body or 'name' not in body or 'score' not in body or 'game' not in body or 'events' not in body:
        return False
    version = body['version']
    id = body['id']
    game = body['game']
    name = body['name']
    score = body['score']
    events = body['events']

    if type(version) is not int or type(id) is not str or type(score) is not int or type(name) is not str or type(
            game) is not int or type(events) is not list:
        return False
    if not re.match(r'[A-Z]{3}', name):
        return False
    if score < 0 or score > 99999:
        return False
    for input_event in events:
        if not validate_input_events(input_event):
            return False

    if 'h' not in params:
        return False

    hash = params['h'][0]
    hashgen = str(sum(bytearray(id + str(game), 'ascii')))
    if hash != hashgen:
        return False

    return True


def cur_day_and_time():
    epoch = int(time.time())
    date = datetime.datetime.fromtimestamp(epoch)
    day = date.strftime('%Y-%m-%d')
    daytime = date.strftime('%H:%M:%S')
    return (day, daytime)


def add_score(request):
    version = request['version']
    id = request['id']
    game = request['game']
    name = request['name']
    score = request['score']
    events = request['events']
    day, daytime = cur_day_and_time()
    padded_score = '0' * (5 - len(str(score))) + str(score)
    item = {
        'pk': {'N': str(version)},
        'sk': {'S': f'{id}/{game}'},
        'name': {'S': name},
        'score': {'N': str(score)},
        'dayscore': {'S': f'{day}/{padded_score}'},
        'time': {'S': daytime},
        'events': {'S': json.dumps(events)},
    }
    client.put_item(TableName='trippin-scores', Item=item)
    print(f'added score, id={id}, name={name}, score={score}')


def convert_item(item):
    sk = item['sk']['S']
    split = sk.split('/')
    id = split[0]
    game = int(split[1])
    name = item['name']['S']
    score = int(item['score']['N'])
    events = json.loads(item['events']['S'])
    return {
        'id': id,
        'game': game,
        'name': name,
        'score': score,
        'events': events
    }


def convert(items):
    return [convert_item(i) for i in items]


def to_response(items):
    return {
        'status': '200',
        'statusDescription': 'OK',
        'headers': {
            'content-type': [
                {
                    'key': 'Content-Type',
                    'value': 'application/json'
                }
            ]
        },
        'body': json.dumps(items)
    }


def extract_version_param(request):
    if 'querystring' not in request:
        return 1

    params = urllib.parse.parse_qs(request['querystring'])
    if 'version' not in params:
        return 1

    version = params['version'][0]
    try:
        return int(version)
    except ValueError:
        return 1


def lambda_handler(event, context):
    request = event['Records'][0]['cf']['request']

    print(f'method={request["method"]}, path={request["uri"]}')

    if request['method'] == 'GET' and '/scores/today' in request['uri']:
        version = extract_version_param(request)
        day, daytime = cur_day_and_time()
        return to_response(convert(top_today_scores(version, day, 25)))

    if request['method'] == 'GET' and '/scores/alltime' in request['uri']:
        version = extract_version_param(request)
        return to_response(convert(top_scores(version, 25)))

    if request['method'] == 'POST' and '/scores' in request['uri']:
        if not validate_add(request):
            return {'status': '400', 'statusDescription': 'Bad Request'}
        add_score(extract_add_body(request))
        return {'status': '200', 'statusDescription': 'OK'}

    return {
        'status': '404',
        'statusDescription': 'Not Found',
        'body': 'Not Found'
    }
