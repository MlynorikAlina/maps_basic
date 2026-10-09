import json
import os
import sys
import argparse

def patch_style(style_path, satellite_dir, vector_mbtiles_path, fonts_dir, sprite):
    if not os.path.exists(style_path):
        print(f"❌ Error: Style file not found at: {style_path}")
        sys.exit(1)

    with open(style_path, 'r', encoding='utf-8') as f:
        try:
            style = json.load(f)
        except json.JSONDecodeError as e:
            print(f"❌ Error parsing JSON: {e}")
            sys.exit(1)

    # Приведение путей к абсолютному формату с заменой Windows-слэшей
    abs_fonts = os.path.abspath(fonts_dir).replace('\\', '/')
    abs_sprite = os.path.abspath(sprite).replace('\\', '/')
    abs_sat = os.path.abspath(satellite_dir).replace('\\', '/')
    abs_vector = os.path.abspath(vector_mbtiles_path).replace('\\', '/')

    # 1. Запись пути к глифам/шрифтам
    style["glyphs"] = f"file://{abs_fonts}/{{fontstack}}/{{range}}.pbf"
    style["sprite"] = f"file://{abs_sprite}"

    # 2. Запись путей к источникам данных
    if "sources" in style:
        # Спутник (Папка с тайлами)
        if "local-satellite" in style["sources"]:
            style["sources"]["local-satellite"]["tiles"] = [
                f"file://{abs_sat}/{{z}}/{{x}}/{{y}}.jpeg"
            ]
            print("✅ Satellite tiles path updated.")
        else:
            print("⚠️ Warning: 'local-satellite' source not found in JSON. Path not modified.")

        # Векторный оверлей (Дороги/Границы .mbtiles)
        if "openmaptiles" in style["sources"]:
            style["sources"]["openmaptiles"]["tiles"] = [f"file://{abs_vector}/{{z}}/{{x}}/{{y}}.pbf"]
            print("✅ Vector MBTiles path updated.")
        else:
            print("⚠️ Warning: 'local-vector-overlay' source not found in JSON. Path not modified.")
    else:
        print("❌ Error: 'sources' block is missing in style.json")
        sys.exit(1)

    # Сохранение результата
    with open(style_path, 'w', encoding='utf-8') as f:
        json.dump(style, f, indent=2, ensure_ascii=False)

    print(f"🎉 Successfully configured style {style_path}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Configure local file paths for mbgl-renderer hybrid_style.json file."
    )

    # Обязательные аргументы для источников данных
    parser.add_argument("--sat", required=True, help="Path to the directory with downloaded satellite tiles")
    parser.add_argument("--vec", required=True, help="Path to the local vector tiles")

    # Аргументы для стилей и шрифтов
    parser.add_argument("-s", "--style", default="./hybrid_style.json", help="Path to input style.json file (default: ./hybrid_style.json)")
    parser.add_argument("-f", "--fonts", default="../data/openmaptiles-fonts/fonts/", help="Path to the directory containing local fonts/glyphs")
    parser.add_argument("-i", "--sprite", default="../data/sprite", help="Path to glyphs sprite.")

    args = parser.parse_args()

    patch_style(
        style_path=args.style,
        satellite_dir=args.sat,
        vector_mbtiles_path=args.vec,
        fonts_dir=args.fonts,
        sprite=args.sprite
    )
