Input ← &fras "input.txt"
Nums ← {"zero" "one" "two" "three" "four"
        "five" "six" "seven" "eight" "nine"}
Lines ← ⊜□≠@\n.
Digits ← (0;|parse)<@a.
Words ← (
  # For each number in Nums we want to check the occurences
  # fix is used on the input to duplicate the array
  ≡⌕Nums¤
  # When we transpose each row is associated with a char
  # from the input
  # There should be a single digit or none in each row
  # Get that digit or a 0 in case there was no match
  ≡(/↥⊂0⊚)⍉
)
PartOne ← (
  # Split input into lines
  Lines
  # Calculate the score for each line
  ⊐≡(
    # Extract the digits, leave 0 everywhere else
    Digits
    # Ignore all 0's, only keep the actual digits
    ⊏⊚.
    # Calculate the number by multiplying the first digit
    # by 10 and adding them
    +⊃(×10⊢|⊢⇌)
  )
  # Sum it all up
  /+
)

# Part two same as one but we use both digits and words
# We combine the two by using max
PartTwo ← /+⊐≡(+⊃(×10⊢|⊢⇌)⊏⊚.↥⊃Digits Words)Lines

&p $"PartOne: _" PartOne Input
&p $"PartTwo: _" PartTwo Input
