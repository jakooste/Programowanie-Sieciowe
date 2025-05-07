import sys
import urllib.request
import json
from urllib.error import URLError, HTTPError

ARTIST_ID = "359282"
URL = f"https://api.discogs.com/artists/{ARTIST_ID}/releases?sort=title&sort_order=asc&per_page=100"
MAX_PAGE_LENGTH = 1000000
MAX_PAGES = 1000


if __name__ == "__main__":

    page = 0
    flag_exit = False
    albums = []

    while not flag_exit:
        page += 1
        req = urllib.request.Request(URL+"&page="+str(page))

        try:
            response = urllib.request.urlopen(req)
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
            pages = response_json['pagination']['pages']
            if pages > MAX_PAGES:
                print(f"!!! Too many pages ({pages})")
                sys.exit(1)
            print(f"Fetched page {page} of {pages}")
            for release in response_json['releases']:
                if 'title' in release and 'role' in release and release['role'] == 'Main':
                    albums.append(release['title'])
            if page == pages:
                print("No more pages to fetch")
                flag_exit = True

        except HTTPError as e:
            print("!!! Server couldn\'t fulfill the request. Error code: ", e.code)
            sys.exit(1)
        except URLError as e:
            print("!!! Failed to reach server. Reason: ", e.reason)
            sys.exit(1)

    print("Albums:")
    for album in albums:
        print(album)
    print(f"Total albums: {len(albums)}")
    