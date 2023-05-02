import json
import boto3
import datetime
import zoneinfo
import re
import base64
import urllib.parse

client = boto3.client('dynamodb', region_name='us-west-2')


def top_scores(version, limit, start_key=None):
    params = {
        'TableName': 'trippin-scores',
        'IndexName': 'score-index',
        'ScanIndexForward': False,
        'Limit': limit,
        'KeyConditionExpression': '#pk = :pk',
        'ExpressionAttributeNames': {'#pk': 'pk'},
        'ExpressionAttributeValues': {':pk': {'N': str(version)}}
    }
    if start_key:
        params['ExclusiveStartKey'] = start_key
    res = client.query(**params)
    return res['Items']


def top_today_scores(version, day, limit, start_key=None):
    params = {
        'TableName': 'trippin-scores',
        'IndexName': 'dayscore-index',
        'ScanIndexForward': False,
        'Limit': limit,
        'KeyConditionExpression': '#pk = :pk AND begins_with (#dayscore, :dayscore)',
        'ExpressionAttributeNames': {'#pk': 'pk', '#dayscore': 'dayscore'},
        'ExpressionAttributeValues': {':pk': {'N': str(version)}, ':dayscore': {'S': day}}
    }
    if start_key:
        params['ExclusiveStartKey'] = start_key
    res = client.query(**params)
    return res['Items']


def extract_add_body(request):
    return json.loads(base64.b64decode(request['body']['data']).decode('utf-8'))


def validate_input_events(events):
    if type(events) is not list:
        return False
    if len(events) % 2 == 1:
        return False
    for n in range(0, len(events), 2):
        tick = events[n]
        input = events[n + 1]
        if type(tick) is not int or type(input) is not int:
            return False
        if tick < 0 or tick > 86_400 * 120 * 7:  # ticks per week (arbitrary ceiling)
            return False
        if input < 0 or input > 15:
            return False
    return True


def validate_add(event):
    if 'body' not in event or 'data' not in event['body'] or 'querystring' not in event:
        return False

    params = urllib.parse.parse_qs(event['querystring'])  # querystring request property is a raw string
    body = extract_add_body(event)
    schema = {
        'version': int,
        'id': str,
        'name': str,
        'score': int,
        'game': int,
        'events': list
    }
    for k, v in schema.items():
        if k not in body:
            return False
        if type(body[k]) != v:
            return False
    version = body['version']
    id = body['id']
    game = body['game']
    name = body['name']
    score = body['score']
    events = body['events']

    if version not in [0, 1]:
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
    date = datetime.datetime.now(tz=zoneinfo.ZoneInfo('America/Los_Angeles'))
    day = date.strftime('%Y-%m-%d')
    daytime = date.strftime('%H:%M:%S')
    return day, daytime


def zero_left_pad(s, size=5):
    return '0' * (size - len(s)) + s


def add_score(request):
    version = request['version']
    id = request['id']
    game = request['game']
    name = request['name']
    score = request['score']
    events = request['events']
    day, daytime = cur_day_and_time()
    padded_score = zero_left_pad(str(score))
    item = {
        'pk': {'N': str(version)},
        'sk': {'S': f'{id}/{game}'},
        'name': {'S': name},
        'score': {'N': str(score)},
        'dayscore': {'S': f'{day}/{padded_score}'},
        'date': {'S': f'{day} {daytime}'},
        'events': {'S': to_json(events)},
    }
    client.put_item(TableName='trippin-scores', Item=item)
    print(f'added score, id={id}, name={name}, score={score}')


def convert_item(item):
    id, game = item['sk']['S'].split('/')
    name = item['name']['S']
    score = item['score']['N']
    events = json.loads(item['events']['S'])
    return {
        'id': id,
        'game': int(game),
        'name': name,
        'score': int(score),
        'events': events
    }


def convert(items):
    return [convert_item(i) for i in items]


def to_json(val):
    return json.dumps(val, separators=(',', ':'))


def compress_if_needed(items):
    while len(j := to_json(items)) >= 40_000:
        del items[-1]  # remove trailing item until under 40 KB limit
    return j


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
        'body': to_json(items)
    }


def extract_param(request, name, default=None, func=None):
    if 'querystring' not in request:
        return default

    params = urllib.parse.parse_qs(request['querystring'])
    if name not in params:
        return default

    val = params[name][0]
    if not func:
        return val if val else default

    try:
        return func(val)
    except ValueError:
        return default


def extract_version_param(request):
    return extract_param(request, 'version', 1, int)


def extract_limit_param(request):
    return extract_param(request, 'limit', 10, int)


def extract_last_id_param(request):
    return extract_param(request, 'last_id')


def extract_last_game_param(request):
    return extract_param(request, 'last_game', None, int)


def extract_last_score_param(request):
    return extract_param(request, 'last_score', None, int)


def extract_last(request):
    last_id = extract_last_id_param(request)
    last_game = extract_last_game_param(request)
    last_score = extract_last_score_param(request)
    return last_id, last_game, last_score


def all_time_start_key(version, last_id, last_game, last_score):
    if last_id and last_game and last_score:
        return {
            'pk': {'N': str(version)},
            'sk': {'S': f'{last_id}/{last_game}'},
            'score': {'N': str(last_score)}
        }


def day_start_key(version, day, last_id, last_game, last_score):
    if last_id and last_game and last_score:
        padded_score = zero_left_pad(str(last_score))
        return {
            'pk': {'N': str(version)},
            'sk': {'S': f'{last_id}/{last_game}'},
            'dayscore': {'S': f'{day}/{padded_score}'}
        }


def lambda_handler(event, context):
    request = event['Records'][0]['cf']['request']

    q = '?' + request['querystring'] if 'querystring' in request else ''
    print(f'method={request["method"]}, path={request["uri"]}{q}')

    if request['method'] == 'GET' and '/scores/today' in request['uri']:
        version = extract_version_param(request)
        limit = extract_limit_param(request)
        day, daytime = cur_day_and_time()
        last_params = extract_last(request)
        start_key = day_start_key(version, day, *last_params)
        return to_response(convert(top_today_scores(version, day, limit, start_key)))

    if request['method'] == 'GET' and '/scores/alltime' in request['uri']:
        version = extract_version_param(request)
        limit = extract_limit_param(request)
        last_params = extract_last(request)
        start_key = all_time_start_key(version, *last_params)
        return to_response(convert(top_scores(version, limit, start_key)))

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
