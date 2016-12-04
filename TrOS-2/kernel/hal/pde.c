// #include <tros/hal/pde.h>
//
// void pde_add_attribute(pde_t* e, unsigned int attrib)
// {
//     *e |= attrib;
// }
//
// void pde_delete_attribute(pde_t* e, unsigned int attrib)
// {
//     *e &= ~attrib;
// }
//
// void pde_set_pte(pde_t* e, unsigned int addr)
// {
//     *e = (*e & ~PDE_PAGE_TABLE) | addr;
// }
//
// int pde_is_present(pde_t e)
// {
//     return e & PDE_PRESENT;
// }
//
// int pde_is_user(pde_t e)
// {
//     return e & PDE_USER;
// }
//
// int pde_is_4mb(pde_t e)
// {
//     return e & PDE_4MB;
// }
//
// int pde_is_writable(pde_t e)
// {
//     return e & PDE_WRITABLE;
// }
//
// unsigned int pde_get_pte_addr(pde_t e)
// {
//     return e & PDE_PAGE_TABLE;
// }
