import pandas as pd
import matplotlib.pyplot as plt

# ---- Option 1: Read from CSV ----
# Replace 'data.csv' with your file name
# df = pd.read_csv('data.csv')

# # Show box plot for all numeric columns
# df.boxplot(figsize=(8, 6))
# plt.title("Box Plot of CSV Data")
# plt.xlabel("Columns")
# plt.ylabel("Values")
# plt.show()

# ---- Option 2: Manual data (if you want to test without CSV) ----
data = [14,16,18,12,13,17,15,19,35,20]
plt.boxplot(data)
plt.title("Box Plot of Sample Data")
plt.ylabel("Values")
plt.show()
