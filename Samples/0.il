set r1, 2
set r2, 4
call @pow
halt

@pow
cmp r2, 0
branch(neq) @continue
set r0, 1
return

@continue
set r0, r1

@loop
cmp r2, 1
return(eq)
sub r2, 1
mul r0, r1
branch @loop
