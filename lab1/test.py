


inpt = """
A 000 AA	13207862122685464576
Z 999 ZZ	7670388314707853312
A 000 AA	4588010303972900864
Z 999 ZZ	12992997081104908288
"""

mp = []

toBeSorted1 = [12, 930, 1, 99, 67, 69]
toBeSorted2 = [10, -1, 1, 3, 3, 5, 1, 3, 2, 4, 9, 0]
toBeSorted3 = ["A 000 AA", "Z 999 ZZ", "A 000 AA", "Z 999 ZZ"]

def pos(x, p):
    s = str(x)
    if p >= len(s):
        return 0
    return ord(s[-1 - p])

def radixSort(toBeSorted):
    maxLen = max(len(x) for x in toBeSorted)
    res = [0] * len(toBeSorted)
    for p in range(maxLen): # 0 1 2
        mx = max([pos(x, p) for x in toBeSorted])
        cnt = [0] * (mx + 1)
        for x in toBeSorted:
            cnt[pos(x, p)] += 1
            # print(pos(x, p))
        # print(cnt)
        for i in range(1, mx + 1):
            cnt[i] += cnt[i - 1]
        # print(cnt)
        for i in range(len(toBeSorted) - 1, -1, -1):
            x = toBeSorted[i]
            res[cnt[pos(x, p)] - 1] = x
            cnt[pos(x, p)] -= 1
        # print(f"res {p}: {res}")
        toBeSorted = res.copy()

    return toBeSorted

print(radixSort(toBeSorted3))


# def countingSort(toBeSorted):
#     res = [0] * len(toBeSorted)
#     maxElement = max(toBeSorted)
#     minElement = min(toBeSorted)
#     cnt = [0] * (maxElement - minElement + 1)
#     for x in toBeSorted:
#         cnt[x - minElement] += 1
#     for x in range(1, len(cnt)):
#         cnt[x] += cnt[x - 1]
#     for i in range(len(toBeSorted) - 1, -1, -1):
#         x = toBeSorted[i]
#         res[cnt[x - minElement] - 1] = x
#         cnt[x - minElement] -= 1
#     return res

# print(countingSort(toBeSorted2))




# while c:=input():

#     key = f"{c.split()[0]} {c.split()[1]} {c.split()[2]}"
#     value = int(c.split()[3])
#     mp.append([key, value])

# mp = sorted(mp, key=lambda lst: lst[0])

# print()
# print(mp)



