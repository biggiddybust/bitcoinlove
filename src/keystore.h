// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_KEYSTORE_H
#define BITCOIN_KEYSTORE_H

#include "key.h"
#include "pubkey.h"
#include "script/script.h"
#include "script/standard.h"
#include "sync.h"
#include "bitcoinlove/Address.hpp"
#include "bitcoinlove/NoteEncryption.hpp"
#include "bitcoinlove/zip32.h"

#include <boost/signals2/signal.hpp>
#include <boost/variant.hpp>

/** A virtual base class for key stores */
class CKeyStore
{
protected:
    mutable CCriticalSection cs_KeyStore;
    mutable CCriticalSection cs_SpendingKeyStore;

public:
    virtual ~CKeyStore() {}

    //! Set the HD seed for this keystore
    virtual bool SetHDSeed(const HDSeed& seed) =0;
    virtual bool HaveHDSeed() const =0;
    //! Get the HD seed for this keystore
    virtual bool GetHDSeed(HDSeed& seedOut) const =0;

    //! Add a key to the store.
    virtual bool AddKeyPubKey(const CKey &key, const CPubKey &pubkey) =0;
    virtual bool AddKey(const CKey &key);

    //! Check whether a key corresponding to a given address is present in the store.
    virtual bool HaveKey(const CKeyID &address) const =0;
    virtual bool GetKey(const CKeyID &address, CKey& keyOut) const =0;
    virtual void GetKeys(std::set<CKeyID> &setAddress) const =0;
    virtual bool GetPubKey(const CKeyID &address, CPubKey& vchPubKeyOut) const;

    //! Support for BIP 0013 : see https://github.com/bitcoin/bips/blob/master/bip-0013.mediawiki
    virtual bool AddCScript(const CScript& redeemScript) =0;
    virtual bool HaveCScript(const CScriptID &hash) const =0;
    virtual bool GetCScript(const CScriptID &hash, CScript& redeemScriptOut) const =0;

    //! Support for Watch-only addresses
    virtual bool AddWatchOnly(const CScript &dest) =0;
    virtual bool RemoveWatchOnly(const CScript &dest) =0;
    virtual bool HaveWatchOnly(const CScript &dest) const =0;
    virtual bool HaveWatchOnly() const =0;

    //! Add a spending key to the store.
    virtual bool AddSproutSpendingKey(const libbitcoinlove::SproutSpendingKey &sk) =0;

    //! Check whether a spending key corresponding to a given payment address is present in the store.
    virtual bool HaveSproutSpendingKey(const libbitcoinlove::SproutPaymentAddress &address) const =0;
    virtual bool GetSproutSpendingKey(const libbitcoinlove::SproutPaymentAddress &address, libbitcoinlove::SproutSpendingKey& skOut) const =0;
    virtual void GetSproutPaymentAddresses(std::set<libbitcoinlove::SproutPaymentAddress> &setAddress) const =0;
    
    //! Add a Sapling spending key to the store.
    virtual bool AddSaplingSpendingKey(
        const libbitcoinlove::SaplingExtendedSpendingKey &sk,
        const libbitcoinlove::SaplingPaymentAddress &defaultAddr) =0;
    
    //! Check whether a Sapling spending key corresponding to a given Sapling viewing key is present in the store.
    virtual bool HaveSaplingSpendingKey(const libbitcoinlove::SaplingFullViewingKey &fvk) const =0;
    virtual bool GetSaplingSpendingKey(const libbitcoinlove::SaplingFullViewingKey &fvk, libbitcoinlove::SaplingExtendedSpendingKey& skOut) const =0;

    //! Support for Sapling full viewing keys
    virtual bool AddSaplingFullViewingKey(
        const libbitcoinlove::SaplingFullViewingKey &fvk,
        const libbitcoinlove::SaplingPaymentAddress &defaultAddr) =0;
    virtual bool HaveSaplingFullViewingKey(const libbitcoinlove::SaplingIncomingViewingKey &ivk) const =0;
    virtual bool GetSaplingFullViewingKey(
        const libbitcoinlove::SaplingIncomingViewingKey &ivk, 
        libbitcoinlove::SaplingFullViewingKey& fvkOut) const =0;
    
    //! Sapling incoming viewing keys 
    virtual bool AddSaplingIncomingViewingKey(
        const libbitcoinlove::SaplingIncomingViewingKey &ivk,
        const libbitcoinlove::SaplingPaymentAddress &addr) =0;
    virtual bool HaveSaplingIncomingViewingKey(const libbitcoinlove::SaplingPaymentAddress &addr) const =0;
    virtual bool GetSaplingIncomingViewingKey(
        const libbitcoinlove::SaplingPaymentAddress &addr, 
        libbitcoinlove::SaplingIncomingViewingKey& ivkOut) const =0;
    virtual void GetSaplingPaymentAddresses(std::set<libbitcoinlove::SaplingPaymentAddress> &setAddress) const =0;

    //! Support for Sprout viewing keys
    virtual bool AddSproutViewingKey(const libbitcoinlove::SproutViewingKey &vk) =0;
    virtual bool RemoveSproutViewingKey(const libbitcoinlove::SproutViewingKey &vk) =0;
    virtual bool HaveSproutViewingKey(const libbitcoinlove::SproutPaymentAddress &address) const =0;
    virtual bool GetSproutViewingKey(
        const libbitcoinlove::SproutPaymentAddress &address,
        libbitcoinlove::SproutViewingKey& vkOut) const =0;
};

typedef std::map<CKeyID, CKey> KeyMap;
typedef std::map<CScriptID, CScript > ScriptMap;
typedef std::set<CScript> WatchOnlySet;
typedef std::map<libbitcoinlove::SproutPaymentAddress, libbitcoinlove::SproutSpendingKey> SproutSpendingKeyMap;
typedef std::map<libbitcoinlove::SproutPaymentAddress, libbitcoinlove::SproutViewingKey> SproutViewingKeyMap;
typedef std::map<libbitcoinlove::SproutPaymentAddress, ZCNoteDecryption> NoteDecryptorMap;

// Full viewing key has equivalent functionality to a transparent address
// When encrypting wallet, encrypt SaplingSpendingKeyMap, while leaving SaplingFullViewingKeyMap unencrypted
typedef std::map<libbitcoinlove::SaplingFullViewingKey, libbitcoinlove::SaplingExtendedSpendingKey> SaplingSpendingKeyMap;
typedef std::map<libbitcoinlove::SaplingIncomingViewingKey, libbitcoinlove::SaplingFullViewingKey> SaplingFullViewingKeyMap;
// Only maps from default addresses to ivk, may need to be reworked when adding diversified addresses. 
typedef std::map<libbitcoinlove::SaplingPaymentAddress, libbitcoinlove::SaplingIncomingViewingKey> SaplingIncomingViewingKeyMap;

/** Basic key store, that keeps keys in an address->secret map */
class CBasicKeyStore : public CKeyStore
{
protected:
    HDSeed hdSeed;
    KeyMap mapKeys;
    ScriptMap mapScripts;
    WatchOnlySet setWatchOnly;
    SproutSpendingKeyMap mapSproutSpendingKeys;
    SproutViewingKeyMap mapSproutViewingKeys;
    NoteDecryptorMap mapNoteDecryptors;

    SaplingSpendingKeyMap mapSaplingSpendingKeys;
    SaplingFullViewingKeyMap mapSaplingFullViewingKeys;
    SaplingIncomingViewingKeyMap mapSaplingIncomingViewingKeys;

public:
    bool SetHDSeed(const HDSeed& seed);
    bool HaveHDSeed() const;
    bool GetHDSeed(HDSeed& seedOut) const;

    bool AddKeyPubKey(const CKey& key, const CPubKey &pubkey);
    bool HaveKey(const CKeyID &address) const
    {
        bool result;
        {
            LOCK(cs_KeyStore);
            result = (mapKeys.count(address) > 0);
        }
        return result;
    }
    void GetKeys(std::set<CKeyID> &setAddress) const
    {
        setAddress.clear();
        {
            LOCK(cs_KeyStore);
            KeyMap::const_iterator mi = mapKeys.begin();
            while (mi != mapKeys.end())
            {
                setAddress.insert((*mi).first);
                mi++;
            }
        }
    }
    bool GetKey(const CKeyID &address, CKey &keyOut) const
    {
        {
            LOCK(cs_KeyStore);
            KeyMap::const_iterator mi = mapKeys.find(address);
            if (mi != mapKeys.end())
            {
                keyOut = mi->second;
                return true;
            }
        }
        return false;
    }
    virtual bool AddCScript(const CScript& redeemScript);
    virtual bool HaveCScript(const CScriptID &hash) const;
    virtual bool GetCScript(const CScriptID &hash, CScript& redeemScriptOut) const;

    virtual bool AddWatchOnly(const CScript &dest);
    virtual bool RemoveWatchOnly(const CScript &dest);
    virtual bool HaveWatchOnly(const CScript &dest) const;
    virtual bool HaveWatchOnly() const;

    bool AddSproutSpendingKey(const libbitcoinlove::SproutSpendingKey &sk);
    bool HaveSproutSpendingKey(const libbitcoinlove::SproutPaymentAddress &address) const
    {
        bool result;
        {
            LOCK(cs_SpendingKeyStore);
            result = (mapSproutSpendingKeys.count(address) > 0);
        }
        return result;
    }

    bool GetSproutSpendingKey(const libbitcoinlove::SproutPaymentAddress &address, libbitcoinlove::SproutSpendingKey &skOut) const
    {
        {
            LOCK(cs_SpendingKeyStore);
            SproutSpendingKeyMap::const_iterator mi = mapSproutSpendingKeys.find(address);
            if (mi != mapSproutSpendingKeys.end())
            {
                skOut = mi->second;
                return true;
            }
        }
        return false;
    }

    bool GetNoteDecryptor(const libbitcoinlove::SproutPaymentAddress &address, ZCNoteDecryption &decOut) const
    {
        {
            LOCK(cs_SpendingKeyStore);
            NoteDecryptorMap::const_iterator mi = mapNoteDecryptors.find(address);
            if (mi != mapNoteDecryptors.end())
            {
                decOut = mi->second;
                return true;
            }
        }
        return false;
    }

    void GetSproutPaymentAddresses(std::set<libbitcoinlove::SproutPaymentAddress> &setAddress) const
    {
        setAddress.clear();
        {
            LOCK(cs_SpendingKeyStore);
            SproutSpendingKeyMap::const_iterator mi = mapSproutSpendingKeys.begin();
            while (mi != mapSproutSpendingKeys.end())
            {
                setAddress.insert((*mi).first);
                mi++;
            }
            SproutViewingKeyMap::const_iterator mvi = mapSproutViewingKeys.begin();
            while (mvi != mapSproutViewingKeys.end())
            {
                setAddress.insert((*mvi).first);
                mvi++;
            }
        }
    }
    
    //! Sapling 
    bool AddSaplingSpendingKey(
        const libbitcoinlove::SaplingExtendedSpendingKey &sk,
        const libbitcoinlove::SaplingPaymentAddress &defaultAddr);
    bool HaveSaplingSpendingKey(const libbitcoinlove::SaplingFullViewingKey &fvk) const
    {
        bool result;
        {
            LOCK(cs_SpendingKeyStore);
            result = (mapSaplingSpendingKeys.count(fvk) > 0);
        }
        return result;
    }
    bool GetSaplingSpendingKey(const libbitcoinlove::SaplingFullViewingKey &fvk, libbitcoinlove::SaplingExtendedSpendingKey &skOut) const
    {
        {
            LOCK(cs_SpendingKeyStore);
            
            SaplingSpendingKeyMap::const_iterator mi = mapSaplingSpendingKeys.find(fvk);
            if (mi != mapSaplingSpendingKeys.end())
            {
                skOut = mi->second;
                return true;
            }
        }
        return false;
    }

    virtual bool AddSaplingFullViewingKey(
        const libbitcoinlove::SaplingFullViewingKey &fvk,
        const libbitcoinlove::SaplingPaymentAddress &defaultAddr);
    virtual bool HaveSaplingFullViewingKey(const libbitcoinlove::SaplingIncomingViewingKey &ivk) const;
    virtual bool GetSaplingFullViewingKey(
        const libbitcoinlove::SaplingIncomingViewingKey &ivk, 
        libbitcoinlove::SaplingFullViewingKey& fvkOut) const;
    
    virtual bool AddSaplingIncomingViewingKey(
        const libbitcoinlove::SaplingIncomingViewingKey &ivk,
        const libbitcoinlove::SaplingPaymentAddress &addr);
    virtual bool HaveSaplingIncomingViewingKey(const libbitcoinlove::SaplingPaymentAddress &addr) const;
    virtual bool GetSaplingIncomingViewingKey(
        const libbitcoinlove::SaplingPaymentAddress &addr, 
        libbitcoinlove::SaplingIncomingViewingKey& ivkOut) const;

    bool GetSaplingExtendedSpendingKey(
        const libbitcoinlove::SaplingPaymentAddress &addr, 
        libbitcoinlove::SaplingExtendedSpendingKey &extskOut) const;
    
    void GetSaplingPaymentAddresses(std::set<libbitcoinlove::SaplingPaymentAddress> &setAddress) const
    {
        setAddress.clear();
        {
            LOCK(cs_SpendingKeyStore);
            auto mi = mapSaplingIncomingViewingKeys.begin();
            while (mi != mapSaplingIncomingViewingKeys.end())
            {
                setAddress.insert((*mi).first);
                mi++;
            }
        }
    }

    virtual bool AddSproutViewingKey(const libbitcoinlove::SproutViewingKey &vk);
    virtual bool RemoveSproutViewingKey(const libbitcoinlove::SproutViewingKey &vk);
    virtual bool HaveSproutViewingKey(const libbitcoinlove::SproutPaymentAddress &address) const;
    virtual bool GetSproutViewingKey(
        const libbitcoinlove::SproutPaymentAddress &address,
        libbitcoinlove::SproutViewingKey& vkOut) const;
};

typedef std::vector<unsigned char, secure_allocator<unsigned char> > CKeyingMaterial;
typedef std::map<CKeyID, std::pair<CPubKey, std::vector<unsigned char> > > CryptedKeyMap;
typedef std::map<libbitcoinlove::SproutPaymentAddress, std::vector<unsigned char> > CryptedSproutSpendingKeyMap;

//! Sapling 
typedef std::map<libbitcoinlove::SaplingExtendedFullViewingKey, std::vector<unsigned char> > CryptedSaplingSpendingKeyMap;

#endif // BITCOIN_KEYSTORE_H
