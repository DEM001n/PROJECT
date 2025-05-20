import numpy as np

#------------------------------------------1.

A = np.random.randint(0, 100, (10, 5))
print(A)

det = np.trace(A)
print("\n",det)

det_values = np.diag(A)
print("\n",det_values)

#------------------------------------------2.

a = np.random.normal(0, 1, 10)
b = np.random.normal(0, 1, 10)
print("\n",a)
print("\n",b)

res = a * b
print("\n",res)

#------------------------------------------3.

a = np.random.randint(0, 10, 20)
b = np.random.randint(0, 10, 20)
print("\n", a)
print("\n", b)

A = a.reshape(-1, 5)
B = b.reshape(-1, 5)
print("\n", A)
print("\n", B)

C = A + B
print("\n", C)

#------------------------------------------4.

A = np.random.randint(0, 10, (4, 5))
B = np.random.randint(0, 10, (5, 4))
print("\n",A)
print("\n",B)

B_t = B.T
print("\n",B_t)

C = A + B_t
print("\n",C)

#------------------------------------------5.

A = np.random.randint(0, 10, (5, 4))
print("\n",A)

res = A[:, 2] * A[:, 3]
print("\n",res)

#------------------------------------------6.

A = np.random.normal(0, 1, (5, 4))
B = np.random.uniform(0, 1, (5, 4)) 
print("\n",A)
print("\n",B)

def stats(matrix):
    print(f"\nŚrednia: {np.mean(matrix)}")
    print(f"Odchylenie standardowe: {np.std(matrix)}")
    print(f"Wariancja: {np.var(matrix)}")
    print(f"Min: {np.min(matrix)}, Max: {np.max(matrix)}")
    print(f"Mediana: {np.median(matrix)}")

stats(A)
stats(B)

#------------------------------------------7.

A = np.random.randint(0, 100, (5, 5))
B = np.random.randint(0, 100, (5, 5))
print("\n",A)
print("\n",B)

res = A * B
print("\n", res)

res_dot = np.dot(A, B)
print("\n", res_dot)

#------------------------------------------8.

A = np.random.randint(0, 100, (5, 5))
print("\n",A)

print("\n",A.strides)

shape = (3, 5)
strides = A.strides

res = np.lib.stride_tricks.as_strided(A, shape, strides)
print("\n",res)

#------------------------------------------9.

a = np.random.randint(0, 100, 5)
b = np.random.randint(0, 100, 5)
print("\n",a)
print("\n",b)

vstack_res = np.vstack((a, b))
print("\n",vstack_res)

hstack_res = np.hstack((a, b))
print("\n",hstack_res)

#-----------------------------------------10.

A = np.arange(24).reshape(4,6)
print("\n",A)

strides = A.strides  
shape = (2, 2, 2, 3) 
new_strides = (strides[0] * 2, strides[1] * 3, strides[0], strides[1])

blocks = np.lib.stride_tricks.as_strided(A, shape, new_strides)
print("\n",blocks)

max = np.max(blocks, (2, 3))
print("\n",max)





