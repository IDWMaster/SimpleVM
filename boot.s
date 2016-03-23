.global _boot

.text
_boot:
# Initialize RAM
mov $0xa00010, %rax
mov 0(%rax), %rax
mov %rax, %rbp
# Allocate a 1MB stack
add $1000000, %rax
mov %rax, %rsp
# Display OS initialization message
mov $start_msg, %rax
mov $0xa00000, %rdi

mov %rax, 0(%rdi)
# TODO: Jump to main method


# Shut down VM
mov $0xa00008, %rax # Address of power management device
mov $0x0, %rdi # Poweroff OPCODE
mov %rdi, 0(%rax) # Write the poweroff OPCODE to the power management device
start_msg:
  .ascii "OS has successfully initialized!\nHello world!\n\0"
stop_msg:
  .ascii "Shutting down VM...."
  