import sys
import urllib.request

def get_server_header(url):

    print(f"{url}:")

    with urllib.request.urlopen("http://" + url) as response:
        print("port 80: " + response.headers.get('Server'))

    with urllib.request.urlopen("https://" + url) as response:
        print("port 443: " + response.headers.get('Server'))

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Sposob uzycia: headers.py [URL]...")
        sys.exit(1)
    
    urls = sys.argv[1:]
    for url in urls:
        get_server_header(url)