import os
import json
import argparse
import re

parser = argparse.ArgumentParser(description="Simple python script to replace file correct style files according to your paths.")

parser.add_argument('-t','--tiles',default="/home/alina/Documents/projects/tile_maps/data/data/",help='set tiles directory absolute path')
parser.add_argument('-f','--fonts',default="/home/alina/Documents/projects/tile_maps/styles/data/openmaptiles-fonts/fonts/",help='set fonts glyphs directory absolute path')
parser.add_argument('-s','--sprite',default="/home/alina/Documents/projects/tile_maps/styles/data/sprite",help='set sprite directory and filename without extension absolute path')

args = parser.parse_args()

if not args.tiles.endswith('/'):
    args.tiles += '/'

if not args.fonts.endswith('/'):
    args.fonts += '/'

source = {
    "metadata": {
        "openmaptiles:version": "3.x"
    },
    "sources":{
        "openmaptiles": {
        "type": "vector",
        #Tiles directory: /home/alina/Documents/projects/tile_maps/data/data/
        "tiles": ["file://" + args.tiles + "{z}/{x}/{y}.pbf"],
        "maxzoom": 14
        }
    },
    "glyphs": "file://" + args.fonts + "{fontstack}/{range}.pbf",
    "sprite": "file://" + args.sprite,
}

if __name__ == "__main__":
    current_dir = os.getcwd()
    print(f"Сканирование директории: {current_dir} и вложенных папок...")

    for root, dirs, files in os.walk(current_dir):
        for file in files:
            if file == "style.json":
                file_path = os.path.join(root, file)
                with open(file_path, 'r',  encoding='utf-8') as f:
                    text_content = f.read()

                cleaned_text = re.sub(r'"Metropolis.*?",\s*', '', text_content)

                data = json.loads(cleaned_text)

                if "metadata" in data and isinstance(data["metadata"], dict):
                    data["metadata"] = source["metadata"]

                data["sources"] = source["sources"]
                data["glyphs"] = source["glyphs"]
                data["sprite"] = source["sprite"]

                with open(file_path, 'w',  encoding='utf-8') as f:
                    json.dump(data,f,indent=2, ensure_ascii=False)


    print(f"Завершено")
