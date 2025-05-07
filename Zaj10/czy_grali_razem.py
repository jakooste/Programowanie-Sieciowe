import sys
import urllib.request
import json
from urllib.error import URLError, HTTPError

MAX_PAGE_LENGTH = 1000000
MAX_PAGES = 1000

def translate_response(response):
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
        print("Sposób użycia: czy_grali_razem.py [ARTYSTA1] [ARTYSTA2]")
        sys.exit(1)
    artist1 = sys.argv[1]
    artist2 = sys.argv[2]
    result = []
    url = f"https://api.discogs.com/artists/{artist1}"

    print(f"Sprawdzam, czy {artist1} i {artist2} grali razem w zespole.")

    with urllib.request.urlopen(url) as response:
        response_json = translate_response(response)
        if 'groups' not in response_json or len(response_json['groups']) == 0:
            print(f"Artysta 1 ({artist1}) nie grał w żadnym zespole.")
            sys.exit(0)
        groups = response_json['groups']
        # print(f"Artysta 1 ({artist1}) grał w {len(groups)} zespołach:")
        for group in groups:
            # print(group['name'])
            response2 = urllib.request.urlopen(group['resource_url'])
            response2_json = translate_response(response2)
            # print(f"Zespół {group['name']} ma {len(response2_json['members'])} członków:")
            for member in response2_json['members']:
                # print(f"{member['name']} ({member['id']})")
                if str(member['id']) == artist2:
                    # print(f"Artysta 2 ({artist2}) grał w zespole {group['name']}.")
                    result.append(group['name'])
    
    if len(result) == 0:
        print(f"Nie grali razem")
    else:
        for group in result:
            print(group)