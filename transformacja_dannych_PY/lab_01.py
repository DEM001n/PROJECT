import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import scipy.stats as scs

#----------------------------------------1.

df = pd.DataFrame({'x':[1, 2, 3, 4, 5], 'y':['a', 'b', 'a', 'b', 'b']})
group = df.groupby('y')
res = group['x'].mean()

print("\n",res)

print("\n"+"-"*40) #----------------------------------------2.

x_count = df['x'].value_counts()
y_count = df['y'].value_counts()

print("\n",x_count)
print("\n",y_count)

print("\n"+"-"*40) #----------------------------------------3.

np_autos = np.loadtxt('autos.csv', delimiter=',', skiprows=1, dtype=str)
df = pd.read_csv('autos.csv')

print("\n",np_autos[:5])
print("\n",df.head())

print("\n"+"-"*40) #----------------------------------------4.

group = df.groupby('make')
group_mean = group[['city-mpg', 'highway-mpg']].mean()
group_mean['average-mpg'] = group_mean.mean(axis=1)

print("\n",group_mean[['average-mpg']])

print("\n"+"-"*40) #----------------------------------------5.

group = df.groupby('make')['fuel-type'].value_counts()

print("\n",group)

print("\n"+"-"*40) #----------------------------------------6.

x = df['length'].dropna()
y = df['city-mpg'].dropna()

coeff_1 = np.polyfit(x, y, 1)
coeff_2 = np.polyfit(x, y, 2)

print("\n", coeff_1)
print("\n", coeff_2)

sorted_x = np.sort(x)
pred_1 = np.polyval(coeff_1, sorted_x)
pred_2 = np.polyval(coeff_2, sorted_x)

print("\n", pred_1[:5])
print("\n", pred_2[:5])

print("\n"+"-"*40) #----------------------------------------7.
'''
x = df['length'].dropna()
y = df['city-mpg'].dropna()
'''

corr, _ = scs.pearsonr(x, y)
print(f'\ncity-mpg a length: {corr}')

#-----------------------------------------------------------8.
'''
x = df['length'].dropna()
y = df['city-mpg'].dropna()

coeff_1 = np.polyfit(x, y, 1)
coeff_2 = np.polyfit(x, y, 2)

sorted_x = np.sort(x)
pred_1 = np.polyval(coeff_1, sorted_x)
pred_2 = np.polyval(coeff_2, sorted_x)
'''

plt.figure(figsize=(8, 6))
plt.scatter(x, y, label="Dane", color="blue", alpha=0.6, s=10)
plt.plot(sorted_x, pred_1, label="Regresja 1 stopnia", color="red")
plt.plot(sorted_x, pred_2, label="Regresja 2 stopnia", color="green")
plt.xlabel("Length")
plt.ylabel("City-mpg")
plt.legend()
plt.show()

#-----------------------------------------------------------9.

x_length = df['length'].dropna()

kde_length = scs.gaussian_kde(x_length)

x_vals_length = np.linspace(min(x_length), max(x_length), 1000)
y_vals_length = kde_length(x_vals_length)

_, ax = plt.subplots(1, 2, figsize=(14, 6))

ax[0].scatter(x_length, np.zeros_like(x_length), label="Próbki (length)", color="blue", alpha=0.6, s=10)
ax[0].plot(x_vals_length, y_vals_length, label="Estymator (length)", color="red", linewidth=2)
ax[0].set_title("Estymator funkcji gęstości: Length")
ax[0].set_xlabel("Length")
ax[0].set_ylabel("Gęstość prawdopodobieństwa")
ax[0].legend()

#-----------------------------------------------------------10.

x_width = df['width'].dropna()

kde_width = scs.gaussian_kde(x_width)

x_vals_width = np.linspace(min(x_width), max(x_width), 1000)
y_vals_width = kde_width(x_vals_width)

ax[1].scatter(x_width, np.zeros_like(x_width), label="Próbki (width)", color="green", alpha=0.6, s=10)
ax[1].plot(x_vals_width, y_vals_width, label="Estymator (width)", color="orange", linewidth=2)
ax[1].set_title("Estymator funkcji gęstości: Width")
ax[1].set_xlabel("Width")
ax[1].set_ylabel("Gęstość prawdopodobieństwa")
ax[1].legend()

plt.tight_layout()
plt.show()

print("\n"+"-"*40) #----------------------------------------11.

x_length = df['length'].dropna()
x_width = df['width'].dropna()

data = np.vstack([x_length, x_width])

kde_2d = scs.gaussian_kde(data)

x_vals = np.linspace(min(x_length), max(x_length), 100)
y_vals = np.linspace(min(x_width), max(x_width), 100)

x,y = np.meshgrid(x_vals, y_vals)
positions = np.vstack([x.ravel(), y.ravel()])
z = kde_2d(positions).reshape(x.shape)

plt.figure(figsize=(8, 6))
plt.contour(x,y,z, levels=10, cmap="Blues")
plt.plot(x_length, x_width, 'r.', label="Próbki", alpha=0.5)
plt.title("Dwuwymiarowy estymator funkcji gęstości")
plt.xlabel("Length")
plt.ylabel("Width")
plt.legend()
plt.savefig("png_plot.png", dpi=300)
plt.savefig("pdf_plot.pdf")
plt.show()
