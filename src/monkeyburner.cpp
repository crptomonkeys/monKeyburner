#include <monkeyburner.hpp>

#include "contract_actions.cpp"

void monkeyburner::maintenace_check() {
  eosio::check(!get_config().get().maintenance, "Contract is in maintenance");
}

void monkeyburner::receive_atomicassets_transfer(
    eosio::name &from, eosio::name &to, std::vector<uint64_t> &asset_ids,
    std::string &memo) {
  if (from == get_self() || to != get_self()) {
    return;
  }

  eosio::check(memo == "burn", "invalid memo");

  maintenace_check();

  auto config = get_config().get();
  auto at = atomicassets::get_assets(get_self());
  auto burnables = get_burnable();

  for (uint64_t asset_id : asset_ids) {
    auto asset = at.require_find(asset_id, "asset has not been received yet");
    auto be =
        burnables.require_find(asset->template_id, "asset is not supported");

    eosio::check(be->capacity > 0, "capacity exhausted");

    // reduce capacity
    burnables.modify(be, get_self(),
                     [&](auto &row) { row.capacity = row.capacity - 1; });

    auto contract_balance = eosiotoken::get_balance(
        be->price.contract, get_self(), be->price.quantity.symbol.code());

    if (contract_balance.amount >= be->price.quantity.amount) {
      eosio::action(eosio::permission_level{get_self(), eosio::name("active")},
                    be->price.contract, eosio::name("transfer"),
                    std::make_tuple(get_self(), from, be->price.quantity,
                                    config.params.memo))
          .send();
    } else {
      eosio::action(
          eosio::permission_level{be->price.contract, eosio::name("active")},
          be->price.contract, eosio::name("issue"),
          std::make_tuple(be->price.contract, be->price.quantity,
                          config.params.memo))
          .send();
      eosio::action(
          eosio::permission_level{be->price.contract, eosio::name("active")},
          be->price.contract, eosio::name("transfer"),
          std::make_tuple(be->price.contract, from, be->price.quantity,
                          config.params.memo))
          .send();
    }

    eosio::action(eosio::permission_level{get_self(), eosio::name("active")},
                  get_self(), eosio::name("logburn"),
                  std::make_tuple(asset_id, from, be->price, be->burn_nft))
        .send();

    if (be->burn_nft) {
      eosio::action(eosio::permission_level{get_self(), eosio::name("active")},
                    eosio::name("atomicassets"), eosio::name("burnasset"),
                    std::make_tuple(get_self(), asset_id))
          .send();
    }
  }
}