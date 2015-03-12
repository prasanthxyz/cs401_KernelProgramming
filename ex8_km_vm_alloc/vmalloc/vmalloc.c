#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <asm/pgtable.h>

#include "print_term.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prasanth P");

char *mem = NULL;
char buf[512];

static pte_t* get_pte (struct mm_struct *mm, unsigned long vaddr, unsigned int *level)
{
    pgd_t *pgd =  NULL;
    pud_t *pud = NULL;
    pmd_t *pmd = NULL;
    pte_t *ptep = NULL;

	*level = PG_LEVEL_NONE;
	pgd = pgd_offset(mm, vaddr);
	if(pgd_none(*pgd))
        return NULL;
	pud = pud_offset(pgd, vaddr);
	if (pud_none(*pud))
        return NULL;

    *level = PG_LEVEL_1G;
    if (pud_large(*pud) || !pud_present(*pud))
        return (pte_t *)pud;
	pmd = pmd_offset(pud, vaddr);
    if (pmd_none(*pmd))
        return NULL;

    *level = PG_LEVEL_2M;
    if(pmd_large(*pmd) || !pmd_present(*pmd))
        return (pte_t *)pmd;

	*level = PG_LEVEL_4K;
    ptep = pte_offset_map(pmd, vaddr);
	if(!ptep) {
		pte_unmap(ptep);
        return NULL;
    }
	return ptep;
}

static int __init myinit(void)
{
    static unsigned long new_page_size, new_page_shift, page_mask, page_offset;
    enum pg_level level;
    unsigned long pa_pamacro, pa_pagewalk, va;
    pte_t *ptep;

    mem = vmalloc(4096);
    va = (unsigned long)mem;
    pa_pamacro = __pa(mem);
    //pa_pagewalk = slow_virt_to_phys(mem);

    /* page walk */
	ptep = get_pte (current->mm, (unsigned long)mem, &level);
	if(!ptep) {
        print_term("Page walk failed.");
	}

#define PTE_SHIFT ilog2(PTRS_PER_PTE)
    new_page_shift = PAGE_SHIFT + (level-1)*PTE_SHIFT;
    new_page_size = 1UL << new_page_shift;
    page_mask = ~(new_page_size - 1);
    page_offset = (unsigned long)mem & ~page_mask;

	pa_pagewalk = ( ((phys_addr_t)pte_pfn(*ptep) << PAGE_SHIFT) | page_offset );
	pte_unmap(ptep);


    sprintf(buf, "Current process pid : %d", current->pid);
    print_term(buf);
    sprintf(buf, "Current process name: %s", current->comm);
    print_term(buf);
    sprintf(buf, "VA                  : %lu", va);
    print_term(buf);
    sprintf(buf, "PA (by __pa macro)  : %lu", pa_pamacro);
    print_term(buf);
    sprintf(buf, "PA (by pagewalk)    : %lu", pa_pagewalk);
    print_term(buf);

    if (pa_pamacro == pa_pagewalk)
        print_term("Physical addresses obtained were SAME.");
    else
        print_term("Physical addresses obtained were DIFFERENT.");
    
    strcpy(buf, "HELLO KERNEL; VMALLOC");
    memcpy(mem, buf, strlen(buf)+1);
    print_term("Wrote \"HELLO KERNEL; VMALLOC\" to the memory location..");

    return 0;
}

static void __exit myexit(void)
{
    vfree(mem);
}

module_init(myinit);
module_exit(myexit);
