import numpy as np

data_types = ['int', 'uint', 'float']
data_sizes = [8, 16, 32, 64]

def generate_test_data():

  npz_dict = {}

  for data_type in data_types:
    for data_size in data_sizes:

      # skip float8
      if data_type == 'float' and data_size == 8:
        continue

      npy_type = data_type + str(data_size)
      type_name = f'k{npy_type.capitalize()}'

      # Empty array
      arr_empty = np.array((), dtype=npy_type)
      npz_dict[f'{type_name}_empty'] = arr_empty

      # Scalar array
      arr_scalar = np.array(0, dtype=npy_type)
      npz_dict[f'{type_name}_rank0'] = arr_scalar

      # Ranks 1 - 5
      for rank in range(1, 6):
        shape = [d + 1 for d in range(rank)]
        arr = np.fromfunction(lambda *args: sum(args), shape, dtype=npy_type)
        npy_name = f'{type_name}_rank{rank}'
        npz_dict[npy_name] = arr

  np.savez('npy_dynamic_array_testdata.npz', **npz_dict)

if __name__ == '__main__':
  generate_test_data()