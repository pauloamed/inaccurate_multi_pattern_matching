import random

N = 10000
M = 1000
MIN_SZ = 2
MAX_SZ = N // 2

chars = "abcd"
alfa_size = len(chars)

patterns = []
for i in range(M):
  sz = random.randint(MIN_SZ, MAX_SZ)
  pattern = ""
  for _ in range(sz):
    x = random.randint(0, alfa_size - 1)
    pattern += chars[x]
  patterns.append(pattern)

str = []
while len(str) < N:
  x = random.randint(0, M - 1)
  i = random.randint(MIN_SZ, len(patterns[x]))
  str += list(patterns[x][:i])

for i in range(len(str)):
  x = random.randint(1, 30)
  if x == 1:
    str[i] = chars[random.randint(0, alfa_size - 1)]

print("".join(str))
print(M)
for pat in patterns:
  print(pat)