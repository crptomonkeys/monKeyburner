#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/system.hpp>

#include <atomicassets-interface.hpp>
#include <eosio.token.hpp>

CONTRACT monkeyburner : public eosio::contract {
public:
  using eosio::contract::contract;

  struct cfg_params {
    std::string memo = "burn NFT reward";
  };

  [[eosio::on_notify("atomicassets::transfer")]] void
      receive_atomicassets_transfer(eosio::name & from, eosio::name & to,
                                    std::vector<uint64_t> & asset_ids,
                                    std::string & memo);

  [[eosio::action]] void setburnable(int32_t template_id,
                                     eosio::extended_asset & price,
                                     bool burn_nft, uint32_t capacity);

  [[eosio::action]] void rmburnable(int32_t template_id);

  [[eosio::action]] void logburn(uint64_t asset_id, eosio::name & account,
                                 eosio::extended_asset & price, bool burned);

  [[eosio::action]] void init();
  [[eosio::action]] void destruct();
  [[eosio::action]] void maintenance(bool maintenance);
  [[eosio::action]] void setparams(cfg_params & params);

private:
  struct [[eosio::table("config")]] _config_entity {
    bool maintenance = true;
    cfg_params params;
  };
  typedef eosio::singleton<eosio::name("config"), _config_entity> _config;

  struct [[eosio::table("burnable")]] _burnable_entity {
    int32_t template_id;
    eosio::extended_asset price;
    bool burn_nft;
    uint32_t capacity;

    auto primary_key() const { return template_id; }
  };
  typedef eosio::multi_index<eosio::name("burnable"), _burnable_entity>
      _burnable;

  _config get_config() { return _config(get_self(), get_self().value); }

  _burnable get_burnable() { return _burnable(get_self(), get_self().value); };

  void maintenace_check();
};