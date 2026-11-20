VERSION ?= all

.PHONY: all us eu split clean progress

all:
	./build.sh $(VERSION)

us:
	./build.sh us

eu:
	./build.sh eu

split:
	python3 -m splat split config/us/wipeout64.yaml
	python3 -m splat split config/eu/wipeout64.yaml

progress:
	python3 tools/progress.py

clean:
	rm -rf asm assets build
