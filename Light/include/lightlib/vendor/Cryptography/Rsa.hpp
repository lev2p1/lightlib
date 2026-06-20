#pragma once

#include "Cryptography.hpp"

    namespace lightlib::crypto {

        struct privateKey {
            crypt_int s; // exponent
            crypt_int p; // prime
            crypt_int q; // prime
        };

        struct publicKey {
            crypt_int r; // exponent
            crypt_int m; // modulus
        };

        class RSA : public Cryptography {
            // private key members
            crypt_int p;
            crypt_int q;
            crypt_int phi; // ϕ(m)

            // public key members
            crypt_int m;
            crypt_int r;

            privateKey* private_key;
            publicKey* public_key;

            void setParameters(crypt_int p, crypt_int q, crypt_int r);
            void evaluateKeys();
            void evaluatePrivateKey();
            void evaluatePublicKey();

            static crypt_int makePositive(crypt_int n, crypt_int mod);
            static crypt_int evaluatePhi(crypt_int a, crypt_int b);

            crypt_int encryptChar(const char &plainchar) const;
            char decryptChar(const crypt_int& cipherchar) const;

        public:
            RSA() = delete;
            RSA(const crypt_int &p, const crypt_int &q, const crypt_int &r);
            ~RSA() override;

            publicKey* getPublicKey() const;
            privateKey* getPrivateKey() const;

            crypt_str encrypt(const std::string& plaintext) override;
            std::string decrypt(const crypt_str& ciphertext) override;
        };
    }
