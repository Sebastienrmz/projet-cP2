"""Construire le ZIP Windows depuis les sources du depot."""
from pathlib import Path
import argparse
import shutil
import subprocess
import zipfile

ROOT = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser()
parser.add_argument('--kind', choices=['images', 'markov', 'game'], required=True)
args = parser.parse_args()
names = {'images': 'Filtres-images-Windows', 'markov': 'Graphes-Markov-Windows', 'game': 'Green-Ball-Windows'}
name = names[args.kind]
stage = ROOT / 'build' / 'windows-package' / name
stage.mkdir(parents=True, exist_ok=True)
if args.kind == 'game':
    subprocess.run(['python', '-m', 'PyInstaller', '--noconfirm', '--clean', '--onedir',
                    '--console', '--name', 'GreenBall', '--add-data', 'assets:assets', 'main.py'], cwd=ROOT, check=True)
    shutil.copytree(ROOT/'dist/GreenBall', stage/'GreenBall', dirs_exist_ok=True)
else:
    build = ROOT/'build/windows'
    subprocess.run(['cmake', '-S', str(ROOT), '-B', str(build), '-DCMAKE_BUILD_TYPE=Release',
                    '-DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded'], check=True)
    subprocess.run(['cmake', '--build', str(build), '--config', 'Release'], check=True)
    source_name = 'clion_projets.exe' if args.kind == 'images' else 'markov_graph.exe'
    exe = next(build.rglob(source_name))
    shutil.copy2(exe, stage/('filtres.exe' if args.kind == 'images' else 'markov_graph.exe'))
    folder = 'images' if args.kind == 'images' else 'data'
    shutil.copytree(ROOT/folder, stage/folder, dirs_exist_ok=True)
shutil.copy2(ROOT/'windows/Tester.bat', stage/'Tester.bat')
shutil.copy2(ROOT/'windows/LIRE-MOI.txt', stage/'LIRE-MOI.txt')
shutil.copy2(ROOT/'README.md', stage/'README.md')
out = ROOT/'dist'
out.mkdir(exist_ok=True)
archive = out/(name+'.zip')
with zipfile.ZipFile(archive, 'w', zipfile.ZIP_DEFLATED) as z:
    for f in sorted(stage.rglob('*')):
        if f.is_file():
            z.write(f, f.relative_to(stage.parent))
print(archive)
