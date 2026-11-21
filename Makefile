VERSION ?= all
PYTHON  ?= python3

.PHONY: all us eu split clean progress

all:
	$(PYTHON) tools/build.py $(VERSION)

us:
	$(PYTHON) tools/build.py us

eu:
	$(PYTHON) tools/build.py eu

split:
	$(PYTHON) -m splat split config/us/wipeout64.yaml
	$(PYTHON) -m splat split config/eu/wipeout64.yaml

progress:
	$(PYTHON) tools/progress.py

clean:
	$(PYTHON) -c "import shutil; [shutil.rmtree(p, ignore_errors=True) for p in ('asm', 'assets', 'build')]"
