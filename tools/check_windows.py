from pathlib import Path
import subprocess
import struct
import tempfile
import shutil

root = Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory(prefix='essai projet ', dir=root/'build') as tmp:
    base = Path(tmp)
    source = root/'build/windows-package/Graphes-Markov-Windows'
    package = base/'Dossier avec espaces'
    shutil.copytree(source, package)
    exe = package/'markov_graph.exe'
    for filename in ['exemple1', 'proba', 'exemple2', 'exemple3']:
        p = subprocess.run([str(exe), f'data/{filename}'], input='1\n2\n4\n7\n8\n10\n0\n3\n5\n0\n', cwd=package, capture_output=True, text=True, timeout=30)
        assert p.returncode == 0, p.stdout+p.stderr
        assert (package/'graph.mmd').is_file()
        assert (package/'hasse.mmd').is_file()

print('OK : executable Windows et ressources testes dans un chemin avec espaces.')
