# Inventory Management System

Inventory management system is a C language based management system that runs on console. This system provides a menu (with specific syntax) to perform either of the below:

1. Manages the inventory system for a grocery store
2. Checks out queues of customers with grocery items

The system uses hash table to manage all the items and queue to checkout a register file.When program runs all the items in inventory.txt is populated to hash table and the items are re-written to the text file from hash table before program ends.Subsequently, menu is displayed in cosole that allowes user to enter specific commands for different operations.

</br>

## Main menu


Main menu offers three options with command syntax

1. Open Inventory submenu

> _inventory_

2. checkout file

> _checkout_ **filename**

3. exit
> _exit_

</br>

**Option 1.** will open a inventory submenu . With this user should be able to manually add, delete, or replenish items in the inventory.
They can either add specific numbers of items back to the inventory, or elect to restock each item that is below its threshold back up to that threshold.

**Option 2.** will take take the name of a .txt file containing customers with grocery lists that are ready to be checked out. The system will check them out, write a receipt, and print any
restock messages that may be necessary.

</br>

### Manage inventory system / inventory submenu

---

This feature provides an inventory submenu to perform following operations:

1. Add items

> add item (syntax: _add_ **key** **name** **threshold** **stock** **price**)

2. Delete items

> delete item (syntax: _delete_ **key**)

3. Restock an item

> restock item (syntax: _restock_ **key** **amount**)

4.  Restock all items under threshold

> restock all (syntax: _restock all_)

5. return to main menu

> return to main menu (syntax: _return_)

<br/>
