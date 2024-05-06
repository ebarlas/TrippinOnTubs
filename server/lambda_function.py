import base64
import datetime
import json
import re
import secrets
import string
import urllib.parse
import zoneinfo

import boto3
import botocore

client = boto3.client('dynamodb', region_name='us-west-2')


def top_scores(version, limit, start_key=None, no_events=False):
    params = {
        'TableName': 'trippin-scores',
        'IndexName': 'score-noevents-index' if no_events else 'score-index',
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


def top_today_scores(version, day, limit, start_key=None, no_events=False):
    params = {
        'TableName': 'trippin-scores',
        'IndexName': 'dayscore-noevents-index' if no_events else 'dayscore-index',
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

    if version < 0 or version > 1000:
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


def random_code():
    alphabet = string.ascii_uppercase + string.digits
    return ''.join(secrets.choice(alphabet) for _ in range(6))


def add_score_with_fallback(request):
    try:
        return add_score(request)
    except botocore.exceptions.ClientError as e:
        if e.response['Error']['Code'] == 'ConditionalCheckFailedException':
            return get_code(request)
        else:
            raise e


def get_code(request):
    version, id, game = (request[k] for k in ['version', 'id', 'game'])
    key = {
        'pk': {'N': str(version)},
        'sk': {'S': f'{id}/{game}'}
    }
    item = client.get_item(TableName='trippin-scores', Key=key, ConsistentRead=True)
    code = item['Item']['code']['S']
    print(f'loaded score, version={version}, id={id}, game={game}, code={code}')
    return code


def add_score(request):
    version = request['version']
    id = request['id']
    game = request['game']
    name = request['name']
    score = request['score']
    events = request['events']
    code = random_code()
    day, daytime = cur_day_and_time()
    padded_score = zero_left_pad(str(score))
    item = {
        'pk': {'N': str(version)},
        'sk': {'S': f'{id}/{game}'},
        'name': {'S': name},
        'score': {'N': str(score)},
        'dayscore': {'S': f'{day}/{padded_score}'},
        'date': {'S': f'{day} {daytime}'},
        'events': {'S': to_json_string(events)},
        'code': {'S': code}
    }
    client.put_item(TableName='trippin-scores', Item=item, ConditionExpression='attribute_not_exists(pk)')
    print(f'added score, version={version}, id={id}, game={game}, name={name}, score={score}, code={code}')
    return code


def convert_item(item):
    id, game = item['sk']['S'].split('/')
    name = item['name']['S']
    score = item['score']['N']
    date = item['date']['S']
    d = {
        'id': id,
        'game': int(game),
        'name': name,
        'score': int(score),
        'date': date
    }
    if 'events' in item:
        d['events'] = json.loads(item['events']['S'])
    return d


def compress_with_diffs(items):
    for item in items:
        if 'events' in item:
            events = item['events']
            cevents = []
            for evts in events:
                cevts = []
                last_tick = 0
                for n, (tick, inpt) in enumerate(zip(evts[0::2], evts[1::2])):
                    tick_diff = tick - last_tick
                    last_tick = tick
                    cevts.append(tick_diff)
                    cevts.append(inpt)
                cevents.append(cevts)
            item['events'] = cevents


def convert(items):
    return [convert_item(i) for i in items]


def to_json_string(val):
    return json.dumps(val, separators=(',', ':'))


def truncate_if_needed(items):
    while len(j := to_json_string(items)) >= 40_000:
        del items[-1]  # remove trailing item until under 40 KB limit
    return j


def to_scores_response(items, compression):
    if compression == 'diff':
        compress_with_diffs(items)
    return to_json_response(truncate_if_needed(items))


def to_json_response(body):
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
        'body': body
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


def extract_no_events_param(request):
    return extract_param(request, 'no_events', False, lambda v: v == 'true')


def extract_compression_param(request):
    return extract_param(request, 'compression')


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
        no_events = extract_no_events_param(request)
        compression = extract_compression_param(request)
        items = convert(top_today_scores(version, day, limit, start_key, no_events))
        return to_scores_response(items, compression)

    if request['method'] == 'GET' and '/scores/alltime' in request['uri']:
        version = extract_version_param(request)
        limit = extract_limit_param(request)
        last_params = extract_last(request)
        start_key = all_time_start_key(version, *last_params)
        no_events = extract_no_events_param(request)
        compression = extract_compression_param(request)
        items = convert(top_scores(version, limit, start_key, no_events))
        return to_scores_response(items, compression)

    if request['method'] == 'POST' and '/scores' in request['uri']:
        if not validate_add(request):
            return {'status': '400', 'statusDescription': 'Bad Request'}
        score_code = add_score_with_fallback(extract_add_body(request))
        return to_json_response(to_json_string({'code': score_code}))

    return {
        'status': '404',
        'statusDescription': 'Not Found',
        'body': 'Not Found'
    }
