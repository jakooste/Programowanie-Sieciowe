import sys
import urllib.request
from urllib.error import URLError, HTTPError

URL = "http://th.if.uj.edu.pl"
CHECK_PHRASE = "Institute of Theoretical Physics"

if __name__ == "__main__":
    req = urllib.request.Request(URL)

    try:
        response = urllib.request.urlopen(req)
        
        print(response.headers)
        if response.status != 200:
            print("!!! Status not OK")  
            sys.exit(1)
        print("Status OK")

        if response.headers.get('Content-type') != "text/html":
            print("!!! Content type not HTML")
            sys.exit(1)
        print("Content HTML")

        the_page = response.read()

        if the_page.find(CHECK_PHRASE) == -1:
            print("!!! Check-phrase not found")
            sys.exit(1)
        print("Check-phrase found")
        
    except HTTPError as e:
        print("!!! Server couldn\'t fulfill the request. Error code: ", e.code)
        sys.exit(1)
    except URLError as e:
        print("!!! Failed to reach server. Reason: ", e.reason)
        sys.exit(1)

    sys.exit(0)