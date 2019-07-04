import numpy as np
import imageio
from collections import deque
import datetime

# Implementation of:
# Pierre Soille, Luc M. Vincent, "Determining watersheds in digital pictures via
# flooding simulations", Proc. SPIE 1360, Visual Communications and Image Processing
# '90: Fifth in a Series, (1 September 1990); doi: 10.1117/12.24211;
# http://dx.doi.org/10.1117/12.24211
class Watershed(object):
   MASK = -2
   WSHD = 0
   INIT = -1
   INQE = -3

   def __init__(self, levels = 256):
      self.levels = levels

   # Neighbour (coordinates of) pixels, including the given pixel.
   def _get_neighbors(self, height, width, pixel):
      return np.mgrid[
         max(0, pixel[0] - 1):min(height, pixel[0] + 2),
         max(0, pixel[1] - 1):min(width, pixel[1] + 2)
      ].reshape(2, -1).T

   def apply(self, image):
      current_label = 0
      flag = False
      fifo = deque()
      result1 = 0.000
      result2 = 0.000
      result3 = 0.000
      

      height, width = image.shape
      total = height * width
      labels = np.full((height, width), self.INIT, np.int32)  # labels = imo

      reshaped_image = image.reshape(total)  # image matrix to vector

      # [y, x] pairs of pixel coordinates of the flattened image.
      pixels = np.mgrid[0:height, 0:width].reshape(2, -1).T  # [ [0 0] [0 1] ... [99 98] [99 99] ]

      # Coordinates of neighbour pixels for each pixel.
      neighbours = np.array([self._get_neighbors(height, width, p) for p in pixels])  # array of neighbors for every pixel p

      if len(neighbours.shape) == 3:
         # Case where all pixels have the same number of neighbours.
         neighbours = neighbours.reshape(height, width, -1, 2)
      else:
         # Case where pixels may have a different number of pixels.
         neighbours = neighbours.reshape(height, width)

      indices = np.argsort(reshaped_image)  # sort indices based on values
      sorted_image = reshaped_image[indices]  # sorted h
      sorted_pixels = pixels[indices]  # sorted p[0, 1]

      # print(sorted_pixels)
      # for a in sorted_pixels:
      #    print("{} - {}".format(a[0]*100+a[1], a[1]*100+a[0]))  # 0 1 eh o certo

      # self.levels evenly spaced steps from minimum to maximum.
      levels = np.linspace(sorted_image[0], sorted_image[-1], self.levels)

      level_indices = []
      current_level = 0

      # Get the indices that deleimit pixels with different values.
      # pega o H e salva a posição que tem valores diferentes
      for i in range(total):
         if sorted_image[i] > levels[current_level]:
            # Skip levels until the next highest one is reached.
            while sorted_image[i] > levels[current_level]:
               current_level += 1
            level_indices.append(i)
            # print("{} - {}".format(sorted_image[i],i))
      level_indices.append(total)

      # for a in level_indices:
      #    print(a)
      #print(len(level_indices))

      start_index = 0
	  
	  # FUNCAO NUMERO 1
      
      for stop_index in level_indices:
         # print(stop_index)
         # Mask all pixels at the current level.

         inicio01 = datetime.datetime.now() 
         
         for p in sorted_pixels[start_index:stop_index]:
            # print("p {} = {} - start {} stop {} - h {}".format(p,p[0]*100+p[1],start_index,stop_index,reshaped_image[p[0]*100+p[1]]))
            labels[p[0], p[1]] = self.MASK
            # Initialize queue with neighbours of existing basins at the current level.
            for q in neighbours[p[0], p[1]]:
               # p == q is ignored here because labels[p] < WSHD
               if labels[q[0], q[1]] >= self.WSHD:
                  labels[p[0], p[1]] = self.INQE
                  fifo.append(p)
                  break

         fim01 = datetime.datetime.now()         
         total1 = (fim01 - inicio01)
         final1 = total1.total_seconds()  
         result1 = result1 + final1
         


		# FUNCAO NUMERO 2
      # Extend basins.
         
         inicio02 = datetime.datetime.now()       
         while fifo:
            p = fifo.popleft()
            # Label p by inspecting neighbours.
            for q in neighbours[p[0], p[1]]:
               # Don't set lab_p in the outer loop because it may change.
               lab_p = labels[p[0], p[1]]
               lab_q = labels[q[0], q[1]]
               if lab_q > 0:
                  if lab_p == self.INQE or (lab_p == self.WSHD and flag):
                     labels[p[0], p[1]] = lab_q
                  elif lab_p > 0 and lab_p != lab_q:
                     labels[p[0], p[1]] = self.WSHD
                     flag = False
               elif lab_q == self.WSHD:
                  if lab_p == self.INQE:
                     labels[p[0], p[1]] = self.WSHD
                     flag = True
               elif lab_q == self.MASK:
                  labels[q[0], q[1]] = self.INQE
                  fifo.append(q)
         
         fim02 = datetime.datetime.now()         
         total2 = (fim02 - inicio02)
         final02 = total2.total_seconds()  
         result2 = result2 + final02       
                  


			# FUNCAO NUMERO 3
         # Detect and process new minima at the current level.
         inicio03 = datetime.datetime.now()
         
         for p in sorted_pixels[start_index:stop_index]:
            # p is inside a new minimum. Create a new label.
            if labels[p[0], p[1]] == self.MASK:
               current_label += 1
               fifo.append(p)
               labels[p[0], p[1]] = current_label
               # print(current_label)
               while fifo:
                  q = fifo.popleft()
                  for r in neighbours[q[0], q[1]]:
                     if labels[r[0], r[1]] == self.MASK:
                        fifo.append(r)
                        labels[r[0], r[1]] = current_label

         start_index = stop_index
         fim03 = datetime.datetime.now()         
         total3 = (fim03 - inicio03)
         final03 = total3.total_seconds()  
         result3 = result3 + final03  
         

      print('Função 01: ',result1)
      print('Função 02: ',result2)
      print('Função 03: ',result3)
      return labels

if __name__ == "__main__":
   import sys
   from PIL import Image
   import matplotlib.pyplot as plt

   w = Watershed()
   image = np.array(Image.open("ex.png")) ## AQUI QUE TROCA A IMAGEM
   labels = w.apply(image)
   img_new = (labels - labels.min()) * ((255 - 0) / (labels.max() - labels.min())) + 0
   imageio.imwrite('ws.png',np.uint8(img_new))
  


