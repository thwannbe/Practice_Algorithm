class ran_gen:
  def __init__(self, a: int, b: int):
    self.seed = 1983
    self.a = a
    self.b = b

  def next(self) -> int:
    ret = self.seed
    self.seed = (self.seed * self.a + self.b) % 20090711
    return ret

class max_heap:
  def __init__(self):
    self.heap = []

  def swap(self, a: int, b: int) -> None:
    tmp = self.heap[b]
    self.heap[b] = self.heap[a]
    self.heap[a] = tmp

  def push(self, num: int) -> None:
    ind = len(self.heap)
    self.heap.append(num)
    parent = int((ind - 1) / 2)
    while ind > 0:
      if self.heap[ind] > self.heap[parent]:
        self.swap(ind,parent)
      ind = parent
      parent = int((ind - 1) / 2)
    #print(self.heap)

  def pop(self) -> int:
    if len(self.heap) == 0:
      return None
    self.swap(0,len(self.heap) - 1)
    ret = self.heap.pop()
    ind = 0
    left = 1
    right = 2
    while left < len(self.heap):
      next = ind
      if self.heap[left] > self.heap[next]:
        next = left
      if right < len(self.heap) and self.heap[right] > self.heap[next]:
        next = right
      if next == ind:
        break
      self.swap(ind,next)
      ind = next
      left = 2 * ind + 1
      right = 2 * ind + 2
    #print(self.heap)
    return ret

class min_heap:
  def __init__(self):
    self.heap = []

  def swap(self, a: int, b: int) -> None:
    tmp = self.heap[b]
    self.heap[b] = self.heap[a]
    self.heap[a] = tmp

  def push(self, num: int) -> None:
    ind = len(self.heap)
    self.heap.append(num)
    parent = int((ind - 1) / 2)
    while ind > 0:
      if self.heap[ind] < self.heap[parent]:
        self.swap(ind,parent)
      ind = parent
      parent = int((ind - 1) / 2)
    #print(self.heap)

  def pop(self) -> int:
    if len(self.heap) == 0:
      return None
    self.swap(0,len(self.heap) - 1)
    ret = self.heap.pop()
    ind = 0
    left = 1
    right = 2
    while left < len(self.heap):
      next = ind
      if self.heap[left] < self.heap[next]:
        next = left
      if right < len(self.heap) and self.heap[right] < self.heap[next]:
        next = right
      if next == ind:
        break
      self.swap(ind,next)
      ind = next
      left = 2 * ind + 1
      right = 2 * ind + 2
    #print(self.heap)
    return ret

def solve(n: int, a: int, b: int) -> int:
  maxh = max_heap()
  minh = min_heap()
  ng = ran_gen(a,b)
  ret = 0
  for i in range(0,n):
    if len(maxh.heap) == len(minh.heap):
      maxh.push(ng.next())
    else:
      minh.push(ng.next())
    if len(maxh.heap) > 0 and len(minh.heap) > 0 and maxh.heap[0] > minh.heap[0]:
      x,y = maxh.pop(),minh.pop()
      maxh.push(y)
      minh.push(x)
    ret += maxh.heap[0]
  return ret % 20090711

print(solve(10,1,0))
print(solve(10,1,1))
print(solve(10000,1273,4936))

