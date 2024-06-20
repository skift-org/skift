# Fetch the entities.json file from the WHATWG HTML spec and print out the
# ENTITY() macro calls for the entities defined in it.


import json
import requests

# download entities.json
url = "https://html.spec.whatwg.org/entities.json"
r = requests.get(url)
entities = json.loads(r.text)
for k, v in entities.items():
    print(f"ENTITY(\"{k}\", {', '.join(map(str, v['codepoints']))})")
