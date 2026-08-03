import os
from cryptography.hazmat.primitives.ciphers.aead import AESGCM

# 1. Génération d'une clé secrète de 256 bits (32 octets)
# En prod, cette clé doit être stockée et gérée de manière sécurisée (HSM, KMS, etc.)
secret_key = AESGCM.generate_key(bit_length=256)
aesgcm = AESGCM(secret_key)

# 2. Le Nonce (Number used ONCE)
# IL NE FAUT JAMAIS RÉUTILISER LE MÊME NONCE AVEC LA MÊME CLÉ.
# Pour AES-GCM, la taille standard est de 12 octets (96 bits).
nonce = os.urandom(12)

# Le message à protéger
message = b"Secret defense : le projet Codespace tourne a fond."

# Données associées (optionnel) : des données en clair que l'on veut authentifier 
# (par ex. un en-tête de paquet, un ID utilisateur) mais sans les chiffrer.
associated_data = b"ID_User: 42"

# 3. Chiffrement + Génération automatique du Tag d'authentification
# Le ciphertext retourné contient à la fois les données chiffrées et le tag à la fin.
ciphertext = aesgcm.encrypt(nonce, message, associated_data)
print(f"Message chiffre (avec tag) : {ciphertext.hex()}")

# 4. Déchiffrement légitime
try:
    decrypted_message = aesgcm.decrypt(nonce, ciphertext, associated_data)
    print(f"Déchiffrement reussi : {decrypted_message.decode('utf-8')}")
except Exception as e:
    print("Erreur de déchiffrement : le message ou le tag a été corrompu !")

# 5. Tentative de piratage / altération (le flux verrouillé en action)
# Si un attaquant modifie ne serait-ce qu'un seul octet dans le ciphertext...
corrupted_ciphertext = bytearray(ciphertext)
corrupted_ciphertext[0] ^= 0x01 # On inverse un bit

try:
    print("\nTentative de déchiffrement du message altéré...")
    aesgcm.decrypt(nonce, bytes(corrupted_ciphertext), associated_data)
except Exception:
    print("ACCÈS REFUSÉ : Le système a détecté la manipulation et a rejeté le bloc instantanément (Pas de fuite, pas d'oracle).")
