CONTRACT=monkeyburner

all: $(CONTRACT).wasm $(CONTRACT).abi

%.wasm: src/%.cpp
	eosio-cpp -Os -I./include -o $@ $<

%.abi: src/%.cpp
	eosio-abigen -o $@ $<

clean:
	rm -f $(CONTRACT).wasm $(CONTRACT).abi
