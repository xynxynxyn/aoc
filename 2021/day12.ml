open Util
open Format
module StringSet = Set.Make (String)
module Cave = Map.Make (String)

let add_connection src tgt cave =
  Cave.update src
    (function
      | Some dests -> Some (StringSet.add tgt dests)
      | None -> Some (StringSet.empty |> StringSet.add tgt))
    cave
  |> Cave.update tgt (function
       | Some dests -> Some (StringSet.add src dests)
       | None -> Some (StringSet.empty |> StringSet.add src))

let make_cave =
  List.fold_left (fun cave (src, tgt) -> add_connection src tgt cave) Cave.empty

let dup_small_cave dup cave =
  let tag = String.concat "" [ "dupe"; dup ] in
  Cave.map
    (fun dests ->
      if StringSet.mem dup dests then StringSet.add tag dests else dests)
    cave
  |> Cave.add tag
       (Cave.find_opt dup cave |> Option.value ~default:StringSet.empty)

let next_caves current visited cave =
  Cave.find_opt current cave |> Option.value ~default:StringSet.empty
  |> fun targets -> StringSet.diff targets visited

let is_lower = function 'a' .. 'z' -> true | _ -> false

let small_caves cave =
  Cave.bindings cave
  |> List.filter_map (fun (k, _) ->
         if k <> "start" && k <> "end" && String.lowercase_ascii k = k then
           Some k
         else None)

let mark_visited tag set =
  if String.lowercase_ascii tag = tag then StringSet.add tag set else set

let update_path tag path =
  let tag =
    if String.starts_with ~prefix:"dupe" tag then
      String.sub tag 4 (String.length tag - 4)
    else tag
  in
  tag :: path

let rec explore_paths path current visited cave =
  let next = next_caves current visited cave in
  if current = "end" then [ List.rev (current :: path) ]
  else if StringSet.is_empty next then []
  else
    (* otherwise branch and visit all the other locations *)
    StringSet.to_seq next |> List.of_seq
    |> List.map (fun target ->
           explore_paths (update_path current path) target
             (mark_visited target visited)
             cave)
    |> List.concat

let main =
  let cave =
    map_lines Sys.argv.(1) (fun s ->
        String.split_on_char '-' s |> fun l -> (List.nth l 0, List.nth l 1))
    |> make_cave
  in
  explore_paths [] "start" (StringSet.empty |> StringSet.add "start") cave
  |> List.length |> printf "part1: %d\n";
  small_caves cave
  |> List.map (fun tag -> dup_small_cave tag cave)
  |> List.map
       (explore_paths [] "start" (StringSet.empty |> StringSet.add "start"))
  |> List.flatten |> List.sort_uniq compare |> List.length
  |> printf "part2: %d\n"
