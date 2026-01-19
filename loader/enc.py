import os

def rol(val, n):
    return ((val << n) & 0xFF) | (val >> (8 - n))

def encrypt(data, seed):
    output = bytearray()
    a = 1664525
    c = 1013904223
    m = 2**32
    state = seed

    for i, byte in enumerate(data):
        state = (a * state + c) % m
        key_stream = (state >> 24) & 0xFF
        
        temp = byte ^ key_stream
        temp = rol(temp, 3)
        temp = (temp + (i & 0xFF)) & 0xFF
        
        output.append(temp)
    return output

input_file = "payload.bin"  # 原始 shellcode
output_file = "cache.dat"
SEED = 0x1337BEEF

if os.path.exists(input_file):
    with open(input_file, "rb") as f:
        original_data = f.read()
    
    encrypted_data = encrypt(original_data, SEED)
    
    with open(output_file, "wb") as f:
        f.write(encrypted_data)
    print(f"Encryption complete: {output_file}")
else:
    print(f"Error: {input_file} not found.")
