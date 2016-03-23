.global _boot

.text
_boot:
# Initialize RAM
mov $0xa00010, %rax # Memory address of RAM is stored in RAX register
mov 0(%rax), %rax
mov %rax, %rbp      # Copy the memory address of the start of RAM into the RBP register (yeah; I know it isn't really meant for that, but whatever)
# Allocate a 1MB stack
add $1000000, %rax   # At 1MB to the RAX register
mov %rax, %rsp       # This is our new stack pointer
# Display OS initialization message
mov $start_msg, %rax	# Load the memory address of start_msg
mov $0xa00000, %rdi	# Load the memory address of the printer device.
mov %rax, 0(%rdi)	# Write the address of our string to the printer, causing the message to be displayed on the printer.

# TODO: If we develop a full-blown OS, we'll insert the call to our main interrupt loop here.


# Display shutdown message
mov $stop_msg, %rax
mov $0xa00000, %rdi
mov %rax, 0(%rdi)

# Shut down VM
mov $0xa00008, %rax # Address of power management device
mov $0x0, %rdi # Poweroff OPCODE
mov %rdi, 0(%rax) # Write the poweroff OPCODE to the power management device
start_msg:
  .ascii "OS has successfully initialized!\nHello world!\n\0"
stop_msg:
  .ascii "Shutting down VM....\n\0"
  