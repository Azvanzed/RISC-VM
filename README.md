This is a small side project i did in about 2 days, nothing much it was just to learn.

![Screenshot 2025-01-28 194706](https://github.com/user-attachments/assets/dba363b7-52eb-48de-b856-0372beda1aab)

Features:
- Compact instructions
- Direct access to IP (Instruction Pointer), CD (Condition aka EFlags in x86)
- Direct GPR portion access (R0.8, R0.4, R0.2...)
- I Don't remember anymore

Usage in debug mode:


```
./Build/Assemblerd_x64 "./Samples/0.il" "./Samples/0.bc"; Build/Interpreterd_x64.exe "./Samples/0.bc"
```
