-- an interesting quickSort implementation in haskell, just for fun

quickSort [] = []
quickSort (x:xs) = quickSort([t|t<-xs, t<x]) ++ [x] ++ quickSort([t|t<-xs, t>=x])

main = do
  let x = [6, 4, 5, 1, 9, 4, 9, 1, 8, 4, 8, 3, 0, 9, 2]
  print x
  print $ quickSort x
