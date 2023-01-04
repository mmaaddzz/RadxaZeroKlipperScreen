// SPDX-License-Identifier: GPL-2.0+
/*
 * DRM driver for display panels connected to a Sitronix ST7715R or ST7735R
 * display controller in SPI mode.
 *
 * Copyright 2017 David Lechner <david@lechnology.com>
 * Copyright (C) 2019 Glider bvba
 * Modified for ST7796 display driver
 */

#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/dma-buf.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/property.h>
#include <linux/spi/spi.h>
#include <video/mipi_display.h>

#include <drm/drm_atomic_helper.h>
#include <drm/drm_drv.h>
#include <drm/drm_fb_helper.h>
#include <drm/drm_gem_cma_helper.h>
#include <drm/drm_gem_framebuffer_helper.h>
#include <drm/drm_managed.h>
#include <drm/drm_mipi_dbi.h>

// #define ST7735R_FRMCTR1		0xb1
// #define ST7735R_FRMCTR2		0xb2
// #define ST7735R_FRMCTR3		0xb3
// #define ST7735R_INVCTR		0xb4
// #define ST7735R_PWCTR1		0xc0
// #define ST7735R_PWCTR2		0xc1
// #define ST7735R_PWCTR3		0xc2
// #define ST7735R_PWCTR4		0xc3
// #define ST7735R_PWCTR5		0xc4
// #define ST7735R_VMCTR1		0xc5
// #define ST7735R_GAMCTRP1	0xe0
// #define ST7735R_GAMCTRN1	0xe1

#define ST7796_MY	BIT(7)
#define ST7796_MX	BIT(6)
#define ST7796_MV	BIT(5)
#define ST7796_RGB	BIT(3)

#define ST7796_CSCON		0xF0






struct st7796_cfg {
	const struct drm_display_mode mode;
	unsigned int left_offset;
	unsigned int top_offset;
	unsigned int write_only:1;
	unsigned int rgb:1;		/* RGB (vs. BGR) */
};

struct st7796_priv {
	struct mipi_dbi_dev dbidev;	/* Must be first for .release() */
	const struct st7796_cfg *cfg;
};

static void st7796_pipe_enable(struct drm_simple_display_pipe *pipe,
				struct drm_crtc_state *crtc_state,
				struct drm_plane_state *plane_state)
{
	struct mipi_dbi_dev *dbidev = drm_to_mipi_dbi_dev(pipe->crtc.dev);
	struct st7796_priv *priv = container_of(dbidev, struct st7796_priv,
						 dbidev);
	struct mipi_dbi *dbi = &dbidev->dbi;
	int ret, idx;
	u8 addr_mode;

	if (!drm_dev_enter(pipe->crtc.dev, &idx))
		return;

	DRM_DEBUG_KMS("\n");

	ret = mipi_dbi_poweron_reset(dbidev);
	if (ret)
		goto out_exit;

	msleep(150);

	mipi_dbi_command(dbi, MIPI_DCS_EXIT_SLEEP_MODE);
	msleep(500);

	mipi_dbi_command(dbi, ST7796_CSCON, 0xC3);
    mipi_dbi_command(dbi, ST7796_CSCON, 0x96);
//    mipi_dbi_command(dbi, MIPI_DCS_SET_ADDRESS_MODE, 0x68);
    mipi_dbi_command(dbi, MIPI_DCS_SET_PIXEL_FORMAT, MIPI_DCS_PIXEL_FMT_16BIT);
    mipi_dbi_command(dbi, 0xB0, 0x80);
    mipi_dbi_command(dbi, 0xB6, 0x00, 0x02);
    mipi_dbi_command(dbi, 0xB5, 0x02, 0x03, 0x00, 0x04);
    mipi_dbi_command(dbi, 0xB1, 0x80, 0x10);
    mipi_dbi_command(dbi, 0xB4, 0x00);
    mipi_dbi_command(dbi, 0xB7, 0xC6);
    mipi_dbi_command(dbi, 0xC5, 0x24);
    mipi_dbi_command(dbi, 0xE4, 0x31);
    mipi_dbi_command(dbi, 0xE8, 0x40, 0x8A, 0x00, 0x00, 0x29, 0x19, 0xA5, 0x33, 0xC2, 0xA7);
    mipi_dbi_command(dbi, 0xE0, 0xF0, 0x09, 0x13, 0x12, 0x12, 0x2B, 0x3C, 0x44, 0x4B, 0x1B, 0x18, 0x17, 0x1D, 0x21);
    mipi_dbi_command(dbi, 0xE1, 0xF0, 0x09, 0x13, 0x0C, 0x0D, 0x27, 0x3B, 0x44, 0x4D, 0x0B, 0x17, 0x17, 0x1D, 0x21);
//    mipi_dbi_command(dbi, MIPI_DCS_SET_ADDRESS_MODE, 0xEC);
    mipi_dbi_command(dbi, 0xF0, 0xC3);
    mipi_dbi_command(dbi, 0xF0, 0x69);
    mipi_dbi_command(dbi, MIPI_DCS_ENTER_NORMAL_MODE);
    mipi_dbi_command(dbi, MIPI_DCS_EXIT_SLEEP_MODE);
//    mipi_dbi_command(dbi, MIPI_DCS_SET_DISPLAY_ON);

//	mipi_dbi_command(dbi, ST7735R_FRMCTR1, 0x01, 0x2c, 0x2d);
//	mipi_dbi_command(dbi, ST7735R_FRMCTR2, 0x01, 0x2c, 0x2d);
//	mipi_dbi_command(dbi, ST7735R_FRMCTR3, 0x01, 0x2c, 0x2d, 0x01, 0x2c,
//			 0x2d);
//	mipi_dbi_command(dbi, ST7735R_INVCTR, 0x07);
//	mipi_dbi_command(dbi, ST7735R_PWCTR1, 0xa2, 0x02, 0x84);
//	mipi_dbi_command(dbi, ST7735R_PWCTR2, 0xc5);
//	mipi_dbi_command(dbi, ST7735R_PWCTR3, 0x0a, 0x00);
//	mipi_dbi_command(dbi, ST7735R_PWCTR4, 0x8a, 0x2a);
//	mipi_dbi_command(dbi, ST7735R_PWCTR5, 0x8a, 0xee);
//	mipi_dbi_command(dbi, ST7735R_VMCTR1, 0x0e);
//	mipi_dbi_command(dbi, MIPI_DCS_EXIT_INVERT_MODE);
	switch (dbidev->rotation) {
	default:
		addr_mode = ST7796_MX;
		break;
	case 90:
		addr_mode = ST7796_MV;
		break;
	case 180:
		addr_mode = ST7796_MY;
		break;
	case 270:
		addr_mode = ST7796_MX | ST7796_MY | ST7796_MV;
		break;
	}

	//if (priv->cfg->rgb)
		addr_mode |= ST7796_RGB;

	mipi_dbi_command(dbi, MIPI_DCS_SET_ADDRESS_MODE, addr_mode);
//	mipi_dbi_command(dbi, MIPI_DCS_SET_PIXEL_FORMAT,
//			 MIPI_DCS_PIXEL_FMT_16BIT);
//	mipi_dbi_command(dbi, ST7735R_GAMCTRP1, 0x02, 0x1c, 0x07, 0x12, 0x37,
//			 0x32, 0x29, 0x2d, 0x29, 0x25, 0x2b, 0x39, 0x00, 0x01,
//			 0x03, 0x10);
//	mipi_dbi_command(dbi, ST7735R_GAMCTRN1, 0x03, 0x1d, 0x07, 0x06, 0x2e,
//			 0x2c, 0x29, 0x2d, 0x2e, 0x2e, 0x37, 0x3f, 0x00, 0x00,
//			 0x02, 0x10);
	mipi_dbi_command(dbi, MIPI_DCS_SET_DISPLAY_ON);

	msleep(100);

	mipi_dbi_command(dbi, MIPI_DCS_ENTER_NORMAL_MODE);

	msleep(20);

	mipi_dbi_enable_flush(dbidev, crtc_state, plane_state);
out_exit:
	drm_dev_exit(idx);
}

static const struct drm_simple_display_pipe_funcs st7796_pipe_funcs = {
	.enable		= st7796_pipe_enable,
	.disable	= mipi_dbi_pipe_disable,
	.update		= mipi_dbi_pipe_update,
};

static const struct st7796_cfg qd40037c1_00_cfg = {
	.mode		= { DRM_SIMPLE_MODE(320, 480, 56, 84) },
	.write_only	= true,
};

DEFINE_DRM_GEM_CMA_FOPS(st7796_fops);

static struct drm_driver st7796_driver = {
	.driver_features	= DRIVER_GEM | DRIVER_MODESET | DRIVER_ATOMIC,
	.fops			= &st7796_fops,
	DRM_GEM_CMA_DRIVER_OPS_VMAP,
	.debugfs_init		= mipi_dbi_debugfs_init,
	.name			= "st7796",
	.desc			= "Sitronix ST7796",
	.date			= "20220630",
	.major			= 1,
	.minor			= 0,
};

static const struct of_device_id st7796_of_match[] = {
	{ .compatible = "quandong,qd40037c1-00", .data = &qd40037c1_00_cfg },
	{ },
};
MODULE_DEVICE_TABLE(of, st7796_of_match);

static const struct spi_device_id st7796_id[] = {
	{ "qd40037c1-00", (uintptr_t)&qd40037c1_00_cfg },
	{ },
};
MODULE_DEVICE_TABLE(spi, st7796_id);

static int st7796_probe(struct spi_device *spi)
{
	struct device *dev = &spi->dev;
	const struct st7796_cfg *cfg;
	struct mipi_dbi_dev *dbidev;
	struct st7796_priv *priv;
	struct drm_device *drm;
	struct mipi_dbi *dbi;
	struct gpio_desc *dc;
	u32 rotation = 0;
	int ret;

	cfg = device_get_match_data(&spi->dev);
	if (!cfg)
		cfg = (void *)spi_get_device_id(spi)->driver_data;

	priv = devm_drm_dev_alloc(dev, &st7796_driver,
				  struct st7796_priv, dbidev.drm);
	if (IS_ERR(priv))
		return PTR_ERR(priv);

	dbidev = &priv->dbidev;
	priv->cfg = cfg;

	dbi = &dbidev->dbi;
	drm = &dbidev->drm;

	dbi->reset = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(dbi->reset)) {
		return dev_err_probe(dev, PTR_ERR(dbi->reset), "Failed to get GPIO 'reset'\n");
	}

	dc = devm_gpiod_get(dev, "dc", GPIOD_OUT_LOW);
	if (IS_ERR(dc)) {
		return dev_err_probe(dev, PTR_ERR(dc), "Failed to get GPIO 'dc'\n");
	}

	dbidev->backlight = devm_of_find_backlight(dev);
	if (IS_ERR(dbidev->backlight))
		return PTR_ERR(dbidev->backlight);

	device_property_read_u32(dev, "rotation", &rotation);

	ret = mipi_dbi_spi_init(spi, dbi, dc);
	if (ret)
		return ret;

	if (cfg->write_only)
		dbi->read_commands = NULL;

	dbidev->left_offset = cfg->left_offset;
	dbidev->top_offset = cfg->top_offset;

	ret = mipi_dbi_dev_init(dbidev, &st7796_pipe_funcs, &cfg->mode,
				rotation);
	if (ret)
		return ret;

	drm_mode_config_reset(drm);

	ret = drm_dev_register(drm, 0);
	if (ret)
		return ret;

	spi_set_drvdata(spi, drm);

	drm_fbdev_generic_setup(drm, 0);

	return 0;
}

static void st7796_remove(struct spi_device *spi)
{
	struct drm_device *drm = spi_get_drvdata(spi);

	drm_dev_unplug(drm);
	drm_atomic_helper_shutdown(drm);
}

static void st7796_shutdown(struct spi_device *spi)
{
	drm_atomic_helper_shutdown(spi_get_drvdata(spi));
}

static struct spi_driver st7796_spi_driver = {
	.driver = {
		.name = "st7796",
		.of_match_table = st7796_of_match,
	},
	.id_table = st7796_id,
	.probe = st7796_probe,
	.remove = st7796_remove,
	.shutdown = st7796_shutdown,
};
module_spi_driver(st7796_spi_driver);

MODULE_DESCRIPTION("Sitronix ST7796 DRM driver");
MODULE_AUTHOR("David Lechner <david@lechnology.com>");
MODULE_LICENSE("GPL");
