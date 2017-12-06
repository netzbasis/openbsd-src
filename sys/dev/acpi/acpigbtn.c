/* $OpenBSD: acpibtn.c,v 1.44 2017/03/02 10:38:10 natano Exp $ */
/*
 * Copyright (c) 2005 Marco Peereboom <marco@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/param.h>
#include <sys/signalvar.h>
#include <sys/systm.h>
#include <sys/device.h>
#include <sys/malloc.h>

#include <machine/bus.h>
#include <machine/apmvar.h>

#include <dev/acpi/acpireg.h>
#include <dev/acpi/acpivar.h>
#include <dev/acpi/acpidev.h>
#include <dev/acpi/amltypes.h>
#include <dev/acpi/dsdt.h>

#include <sys/sensors.h>

int	acpigbtn_match(struct device *, void *, void *);
void	acpigbtn_attach(struct device *, struct device *, void *);
int	acpigbtn_notify(struct aml_node *, int, void *);
int	acpigbtn_activate(struct device *, int);

struct acpigbtn_softc {
	struct device		sc_dev;

	bus_space_tag_t		sc_iot;
	bus_space_handle_t	sc_ioh;

	struct acpi_softc	*sc_acpi;
	struct aml_node		*sc_devnode;

	struct ksensor		sc_sens;
	struct ksensordev	sc_sensdev;

	int			sc_count;
};

struct cfattach acpigbtn_ca = {
	sizeof(struct acpigbtn_softc), acpigbtn_match, acpigbtn_attach, NULL,
	acpigbtn_activate
};

struct cfdriver acpigbtn_cd = {
	NULL, "acpigbtn", DV_DULL
};

const char *acpigbtn_hids[] = { ACPI_DEV_GBTN, 0 };

int
acpigbtn_match(struct device *parent, void *match, void *aux)
{
	struct acpi_attach_args	*aa = aux;
	struct cfdata		*cf = match;

	/* sanity */
	return (acpi_matchhids(aa, acpigbtn_hids, cf->cf_driver->cd_name));
}

void
acpigbtn_attach(struct device *parent, struct device *self, void *aux)
{
	struct acpigbtn_softc	*sc = (struct acpigbtn_softc *)self;
	struct acpi_attach_args *aa = aux;
	int64_t			st;

	sc->sc_acpi = (struct acpi_softc *)parent;
	sc->sc_devnode = aa->aaa_node;

	printf(": %s\n", sc->sc_devnode->name);

	if (aml_evalinteger(sc->sc_acpi, sc->sc_devnode, "_STA", 0, NULL, &st))
		st = STA_PRESENT | STA_ENABLED | STA_DEV_OK;
	if ((st & (STA_PRESENT | STA_ENABLED | STA_DEV_OK)) !=
	    (STA_PRESENT | STA_ENABLED | STA_DEV_OK))
		return;

	aml_register_notify(sc->sc_devnode, aa->aaa_dev, acpigbtn_notify,
	    sc, ACPIDEV_NOPOLL);
}

int
acpigbtn_notify(struct aml_node *node, int notify_type, void *arg)
{
	struct acpigbtn_softc	*sc = arg;

	printf("acpigbtn_notify: %.2x %s\n", notify_type,
	    sc->sc_devnode->name);

	return (0);
}

int
acpigbtn_activate(struct device *self, int act)
{
	/*
	struct acpigbtn_softc	*sc = (struct acpigbtn_softc *)self;
	*/

	switch (act) {
	case DVACT_SUSPEND:
		printf("acpigbtn_activate: suspend\n");
		break;
	case DVACT_WAKEUP:
		printf("acpigbtn_activate: wakeup\n");	
		break;
	}
	return (0);
}
