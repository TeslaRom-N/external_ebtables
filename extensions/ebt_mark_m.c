#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <getopt.h>
#include "../include/ebtables_u.h"
#include <linux/netfilter_bridge/ebt_mark_m.h>

#define MARK '1'

static struct option opts[] =
{
	{ "mark"     , required_argument, 0, MARK },
	{ 0 }
};

static void print_help()
{
	printf(
"mark option:\n"
"--mark    [!] value[/mask]: Match nfmask value with optional mask\n");
}

static void init(struct ebt_entry_match *match)
{
	struct ebt_mark_m_info *markinfo = (struct ebt_mark_m_info *)match->data;

	markinfo->mark = 0;
	markinfo->mask = 0;
	markinfo->invert = 0;
}

#define OPT_MARK 0x01
static int parse(int c, char **argv, int argc, const struct ebt_u_entry *entry,
   unsigned int *flags, struct ebt_entry_match **match)
{
	struct ebt_mark_m_info *markinfo = (struct ebt_mark_m_info *)
	   (*match)->data;
	char *end;

	switch (c) {
	case MARK:
		check_option(flags, MARK);
		if (check_inverse(optarg))
			markinfo->invert = 1;
		if (optind > argc)
			print_error("No mark specified");
		markinfo->mark = strtoul(argv[optind - 1], &end, 0);
		if (*end == '/')
			markinfo->mask = strtoul(end+1, &end, 0);
		else
			markinfo->mask = 0xffffffff;
		if ( *end != '\0' || end == argv[optind - 1])
			print_error("Bad mark value '%s'", argv[optind - 1]);
		break;
	default:
		return 0;
	}
	return 1;
}

static void final_check(const struct ebt_u_entry *entry,
   const struct ebt_entry_match *match, const char *name,
   unsigned int hook_mask, unsigned int time)
{
}

static void print(const struct ebt_u_entry *entry,
   const struct ebt_entry_match *match)
{
	struct ebt_mark_m_info *markinfo =
	   (struct ebt_mark_m_info *)match->data;

	printf("--mark ");
	if (markinfo->invert)
		printf("! ");
	if(markinfo->mask != 0xffffffff)
		printf("0x%lx/0x%lx ", markinfo->mark, markinfo->mask);
	else
		printf("0x%lx ", markinfo->mark);
}

static int compare(const struct ebt_entry_match *m1,
   const struct ebt_entry_match *m2)
{
	struct ebt_mark_m_info *markinfo1 = (struct ebt_mark_m_info *)m1->data;
	struct ebt_mark_m_info *markinfo2 = (struct ebt_mark_m_info *)m2->data;

	if (markinfo1->invert != markinfo2->invert)
		return 0;
	if (markinfo1->mark != markinfo2->mark)
		return 0;
	if (markinfo1->mask != markinfo2->mask)
		return 0;
	return 1;
}

static struct ebt_u_match mark_match =
{
	EBT_MARK_MATCH,
	sizeof(struct ebt_mark_m_info),
	print_help,
	init,
	parse,
	final_check,
	print,
	compare,
	opts,
};

static void _init(void) __attribute((constructor));
static void _init(void)
{
	register_match(&mark_match);
}