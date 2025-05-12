import sys
import urllib.request
import json
from urllib.error import URLError, HTTPError

MAX_PAGE_LENGTH = 1000000
MAX_PAGES = 1000

AGENT_NAME = "Yako's Discogs API client +https://github.com/jakooste/PS"

def translate_response_to_json(response):
    if response.status != 200:
        print("!!! Status not OK")
        sys.exit(1)
    if response.headers.get('Content-Type') != "application/json":
        print("!!! Unexpected content type")
        sys.exit(1)
    response_decoded = response.read().decode("utf-8")
    if len(response_decoded) > MAX_PAGE_LENGTH:
        print(f"!!! Page too long ({len(response_decoded)})")
        sys.exit(1)
    response_json = json.loads(response_decoded)
    return response_json

if __name__ == "__main__":

    if len(sys.argv) < 3:
        print("Sposób użycia: czy_grali_razem.py [ARTYSTA1_ID] [ARTYSTA2_ID]")
        sys.exit(1)
    
    artist1 = sys.argv[1]
    artist2 = sys.argv[2]
    result = []
    url = f"https://api.discogs.com/artists/{artist1}"

    request = urllib.request.Request(url, headers={'User-Agent': AGENT_NAME})
    with urllib.request.urlopen(request) as response:
        
        response_json = translate_response_to_json(response)

        if 'groups' not in response_json or len(response_json['groups']) == 0:
            print(f"Artysta 1 ({artist1}) nie grał w żadnym zespole.")
            sys.exit(0)
        
        groups = response_json['groups']

        for group in groups:
            request2 = urllib.request.Request(group['resource_url'], headers={'User-Agent': AGENT_NAME})
            with urllib.request.urlopen(request2) as response2:
                response2_json = translate_response_to_json(response2)
                for member in response2_json['members']:
                    if str(member['id']) == artist2:
                        result.append(group['name'])
    
    if len(result) == 0:
        print(f"Nie grali razem")
    else:
        print(f"Grali razem w:")
        result.sort()
        for group in result:
            print(group)