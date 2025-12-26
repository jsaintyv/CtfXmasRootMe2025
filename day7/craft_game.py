#!/usr/bin/env python3
# Wii Root-Me Edition — internal signing tool
# WARNING: DO NOT DISTRIBUTE OUTSIDE DEV TEAM

import sys
from Cryptodome.Signature import pkcs1_15
from Cryptodome.PublicKey import RSA
from Cryptodome.Hash import SHA256


file_to_sign = sys.argv[1]

# --- Read game binary ---
with open(file_to_sign, "rb") as f:
    data = f.read()


with open("games/super_smash_bros-brawl_rootme_arena", "rb") as f:
    f.seek(-256, 2)
    validSigned = f.read(256)

# --- Compute SHA-256 hash (PyCryptodome) ---
for i in range(2 << 32):
    dataExtended = data + i.to_bytes(8, byteorder="big", signed=False)    
    hash_obj = SHA256.new(dataExtended).digest()    
    if hash_obj[0] == 0 and hash_obj[1] == 0: 

        extendedFile = file_to_sign + "_extended"
        with open(extendedFile, "wb") as f:
            f.write(dataExtended)
        dataExtended += validSigned

        signedFile = file_to_sign + "_signed"
        with open(signedFile, "wb") as f:
            f.write(dataExtended)
        print(f"signedFile:${signedFile} at index {i}")
        sys.exit()

print(f"Failed found to expected SHA256 starting by 0")

