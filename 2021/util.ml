let fold_lines file f =
  let in_ch = open_in file in
  let rec loop () =
    try
      let next = input_line in_ch in
      f next :: loop ()
    with End_of_file ->
      close_in in_ch;
      []
  in
  loop ()

