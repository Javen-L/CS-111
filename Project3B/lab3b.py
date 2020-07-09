#NAME: Shrenik Manoj Kankaria,Dhruv Singhania
#EMAIL: shrenik0123@gmail.com,singhania_dhruv@yahoo.com
#ID: 005176857,105125631


#modules to handle command line args and csv functions
import sys
import csv
import math


#arrays to hold different line types
superblock_summary = None 
group_summary = None
free_block_entries = []
free_inode_entries = []
inode_summaries = []
directory_entries = []
indirect_block_references = []
allocated_blocks = {}
duplicates = set()


#class definitions
class superblock:
    def __init__(self, field):
        self.total_blocks = int(field[1])
        self.total_inodes = int(field[2])
        self.block_size = int(field[3])
        self.inode_size = int(field[4])
        self.blocks_per_group = int(field[5])
        self.inodes_per_group = int(field[6])
        self.first_inode = int(field[7])

class group:
    def __init__(self, field):
        self.group_number = int(field[1])
        self.total_blocks = int(field[2])
        self.total_inodes = int(field[3])
        self.free_blocks = int(field[4])
        self.free_inodes = int(field[5])
        self.free_block_bitmap = int(field[6])
        self.free_inode_bitmap = int(field[7])
        self.first_block_inode = int(field[8])

class inode:
    def __init__(self, field):
        self.inode_number = int(field[1])
        self.file_type = field[2]
        self.mode = field[3]
        self.owner = int(field[4])
        self.group = int(field[5])
        self.link_count = int(field[6])
        self.change_time = field[7]
        self.modification_time = field[8]
        self.access_time = field[9]
        self.file_size = int(field[10])
        self.disk_space = int(field[11])
        self.direct_blocks = [int(index) for index in field[12:24]]
        self.indirect_blocks = [int(index) for index in field[24:27]]

class dirent:
    def __init__(self, field):
        self.parent_inode = int(field[1])
        self.logical_offset = int(field[2])
        self.referenced_inode = int(field[3])
        self.entry_length = int(field[4])
        self.name_length = int(field[5])
        self.name = str(field[6])

class indirect:
    def __init__(self, field):
        self.owning_inode = int(field[1])
        self.indirection_level = field[2]
        self.block_offset = field[3]
        self.scanned_block_number = int(field[4])
        self.referenced_block_number = int(field[5])


#helper function to handle errors
def handle_error(msg):
    sys.stderr.write(msg)
    sys.exit(1)


#Checks for invalid and reserved indirect blocks. Also places duplicate blocks in a list and creates another bitmap.
def check(total_blocks, first_legal_block, level_str, num_block, inode_num, offset):
    #gives allocated_blocks and duplicates global scope
    global allocated_blocks
    global duplicates
    #checks for invalid block
    if num_block < 0 or num_block >= total_blocks:
        print("INVALID {} BLOCK {} IN INODE {} AT OFFSET {}".format(level_str, num_block, inode_num, offset))
    #checks for reserved block
    elif num_block > 0 and num_block < first_legal_block:
        print("RESERVED {} BLOCK {} IN INODE {} AT OFFSET {}" .format(level_str, num_block, inode_num, offset))
    #checks if block is used
    elif num_block != 0:
        #if block is already in the list, needs to be added as a duplicate
        if num_block in allocated_blocks:
            duplicates.add(num_block)
            allocated_blocks[num_block].append([inode_num, offset, level_str])
        else:
            #if not in bitmap/list yet, add block to it
            allocated_blocks[num_block] = [[inode_num, offset, level_str]]
        

#does the block consistency audit
def block_consistency_audit(total_blocks, first_legal_block):
    for num_inode in inode_summaries:
        #checks for small symbolic links and continues
        if num_inode.file_type == 's' and num_inode.file_size <= 60:
            continue
        offset = 0
        for num_block in num_inode.direct_blocks:
            #check for direct blocks
            if num_block < 0 or num_block >= total_blocks:
                print("INVALID BLOCK {} IN INODE {} AT OFFSET {}".format(num_block, num_inode.inode_number, offset))
            #check for reserved block
            elif num_block > 0 and num_block < first_legal_block:
                print("RESERVED BLOCK {} IN INODE {} AT OFFSET {}" .format(num_block, num_inode.inode_number, offset))
            #check if block is not used
            elif num_block != 0:
                #if already in bitmap/list, add to duplicates
                if num_block in allocated_blocks:
                    duplicates.add(num_block)
                    allocated_blocks[num_block].append([num_inode.inode_number, offset, ""])
                else:
                    #add to bitmap/list if not already there
                    allocated_blocks[num_block] = [[num_inode.inode_number, offset, ""]]
            offset += 1

        #checks for indirect blocks
        offset = 0
        #set correct offset and values indicating indirection level
        for i in range(0, 3):
            if i == 0:
                offset = 12
                level_str = "INDIRECT"
            if i == 1:
                offset = 268
                level_str = "DOUBLE INDIRECT"
            if i == 2:
                offset = 65804
                level_str = "TRIPLE INDIRECT"
            num_block = num_inode.indirect_blocks[i]
            #print our information regarding the indirect blocks: invalid, reserved, add duplicates and complete bitmap/list
            check(total_blocks, first_legal_block, level_str, num_block, num_inode.inode_number, offset)

    #checks for indirect blocks within other indirect blocks
    offset = 0
    for indirect_block in indirect_block_references:
        num_block = indirect_block.referenced_block_number
        num_inode = indirect_block.owning_inode
        level_str = ""
        #Check indirection level and set string accordingly
        if indirect_block.indirection_level == 1:
            lvel_str = "INDIRECT"
        elif indirect_block.indirection_level == 2:
            lvel_str = "DOUBLE INDIRECT"
        elif indirect_block.indirection_level == 3:
            lvel_str = "TRIPLE INDIRECT"
        #print our information regarding the indirect blocks: invalid, reserved, add duplicates and complete bitmap/list
        check(total_blocks, first_legal_block, level_str, num_block, num_inode, indirect_block.block_offset)
        
    #check for unreferenced blocks and allocated blocks on the freelist
    for num_block in range(first_legal_block, (total_blocks)):
        #checks for unreferenced block
        if num_block not in allocated_blocks and num_block not in free_block_entries:
            print("UNREFERENCED BLOCK {}".format(num_block))
        #checks for allocated block
        if num_block in allocated_blocks and num_block in free_block_entries:
            print("ALLOCATED BLOCK {} ON FREELIST".format(num_block))

    #checks for duplicates
    for num_block in duplicates:
        for dup in allocated_blocks[num_block]:
            level_str = dup[2]
            if level_str == "":
                print("DUPLICATE BLOCK {} IN INODE {} AT OFFSET {}".format(num_block, dup[0], dup[1]))
            else:
                print("DUPLICATE {} BLOCK {} IN INODE {} AT OFFSET {}".format(level_str, num_block, dup[0], dup[1]))
            
        
#helper function to handle inode allocation audit
def handle_inode_allocation_audit(first_inode, total_inodes, allocated_inode_entries):
    #checks for allocated inodes in the freelist
    for inode in inode_summaries:
        if inode.inode_number in free_inode_entries:
            print("ALLOCATED INODE {} ON FREELIST".format(inode.inode_number))

    #checks for unallocated inodes not in freelist
    for inode in range(first_inode, total_inodes):
        if inode not in free_inode_entries and inode not in allocated_inode_entries:
            print("UNALLOCATED INODE {} NOT ON FREELIST".format(inode))


#helper function to handle directory consistency audit
def handle_directory_consistency_audit(total_inodes, allocated_inode_entries):
    linkcount = {} #dict to hold inode linkcount
    parent_entries = {} #dict to hold inode parent
    for i in range(1, total_inodes + 1): #fills both dicts with 0s
        linkcount[i] = 0
        parent_entries[i] = 0
    parent_entries[2] = 2

    for entry in directory_entries: 
        #reference to an invalid inode
        if entry.referenced_inode < 1 or entry.referenced_inode > total_inodes:
            print("DIRECTORY INODE {} NAME {} INVALID INODE {}".format(entry.parent_inode, entry.name, entry.referenced_inode))
        #reference to an unallocated inode
        elif entry.referenced_inode not in allocated_inode_entries:
            print("DIRECTORY INODE {} NAME {} UNALLOCATED INODE {}".format(entry.parent_inode, entry.name, entry.referenced_inode))
        #update linkcount
        else:
            linkcount[entry.referenced_inode] += 1

    #inode reference count doesn't match linkcount
    for inode in inode_summaries:
        if inode.link_count != linkcount[inode.inode_number]:
            print("INODE {} HAS {} LINKS BUT LINKCOUNT IS {}".format(inode.inode_number, linkcount[inode.inode_number], inode.link_count))

    #update parent inodes
    for entry in directory_entries:
        if entry.name != "'..'" and entry.name != "'.'":
            parent_entries[entry.referenced_inode] = entry.parent_inode

    #checks correctness of '.' and '..' links
    for entry in directory_entries:
        if entry.parent_inode != entry.referenced_inode and entry.name == "'.'":
            print("DIRECTORY INODE {} NAME '.' LINK TO INODE {} SHOULD BE {}".format(entry.parent_inode, entry.referenced_inode, entry.parent_inode))
        elif parent_entries[entry.parent_inode] != entry.referenced_inode and entry.name == "'..'":
            print("DIRECTORY INODE {} NAME '..' LINK TO INODE {} SHOULD BE {}".format(entry.parent_inode, entry.referenced_inode, parent_entries[entry.parent_inode]))


def main():
    #Print usage message
    if len(sys.argv) != 2: #check correct num of args
        handle_error("Error with number of args. Usage is ./lab3b [filename].\n")

    try:
        csv_file = open(sys.argv[1], "r") #open file
    except:
        handle_error("Error opening file.\n") #error opening file

    csv_reader = csv.reader(csv_file, skipinitialspace=True) #stores file by line
    for field in csv_reader: #sort lines by type
        if not field: #skips blank lines
            continue
        elif field[0] == "SUPERBLOCK":
            superblock_summary = superblock(field)
        elif field[0] == "GROUP":
            group_summary = group(field)
        elif field[0] == "BFREE":
            free_block_entries.append(int(field[1]))
        elif field[0] == "IFREE":
            free_inode_entries.append(int(field[1]))
        elif field[0] == "INODE":
            inode_summaries.append(inode(field))
        elif field[0] == "DIRENT":
            directory_entries.append(dirent(field))
        elif field[0] == "INDIRECT":
            indirect_block_references.append(indirect(field))
        else:
            handle_error("Error with file element.\n")

    first_legal_block_num = int(group_summary.first_block_inode + math.ceil(superblock_summary.inode_size*group_summary.total_inodes/superblock_summary.block_size))
    allocated_inode_entries = [] #holds allocated inode list
    for i in inode_summaries:
        if i.inode_number != 0:
            allocated_inode_entries.append(i.inode_number)

    block_consistency_audit(superblock_summary.total_blocks, first_legal_block_num)
    handle_inode_allocation_audit(superblock_summary.first_inode, superblock_summary.total_inodes, allocated_inode_entries)
    handle_directory_consistency_audit(superblock_summary.total_inodes, allocated_inode_entries)

if __name__ == '__main__':
    main()

