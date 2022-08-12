void monkeyburner::setburnable(int32_t template_id,
                               eosio::extended_asset &price, bool burn_nft,
                               uint32_t capacity) {
  require_auth(get_self());

  eosio::check(price.quantity.amount > 0, "price must be positive");

  auto burnables = get_burnable();

  auto be = burnables.find(template_id);

  auto r = (_burnable_entity){.template_id = template_id,
                              .price = price,
                              .burn_nft = burn_nft,
                              .capacity = capacity};
  if (be == burnables.end()) {
    burnables.emplace(get_self(), [&r = r](auto &row) { row = r; });
  } else {
    burnables.modify(be, get_self(), [&r = r](auto &row) { row = r; });
  }
}

void monkeyburner::rmburnable(int32_t template_id) {
  require_auth(get_self());

  auto burnables = get_burnable();
  auto be = burnables.find(template_id);

  if (be != burnables.end()) {
    burnables.erase(be);
  }
}

void monkeyburner::init() {
  require_auth(get_self());
  get_config().remove();

  get_config().set(_config_entity{}, get_self());
}

void monkeyburner::destruct() {
  require_auth(get_self());
  get_config().remove();
}

void monkeyburner::maintenance(bool maintenance) {
  require_auth(get_self());

  auto config = get_config();
  auto new_config = config.get();

  new_config.maintenance = maintenance;

  config.set(new_config, get_self());
}

void monkeyburner::setparams(cfg_params &params) {
  require_auth(get_self());
  auto config = get_config();
  auto new_config = config.get();

  new_config.params = params;

  config.set(new_config, get_self());
}

void monkeyburner::logburn(uint64_t asset_id, eosio::name &account,
                           eosio::extended_asset &pricei, bool burned) {
  require_auth(get_self());
}